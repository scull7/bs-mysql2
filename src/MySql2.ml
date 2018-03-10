module Connection = struct

  type t

  module Config = struct
    type t

    external make :
      ?host:string ->
      ?port:int ->
      ?user:string ->
      ?password:string ->
      ?database:string ->
      unit ->t = "" [@@bs.obj]
  end

  external createConnection : Config.t -> t = "" [@@bs.module "mysql2" ]
  external close : t -> unit = "end" [@@bs.send]

  let make ?host ?port ?user ?password ?database _ =
    createConnection
      (Config.make ?host ?port ?user ?password ?database ())
end

module Options = struct
  type t = <
    sql: string;
    values: Js.Json.t Js.Nullable.t;
    namedPlaceholders: Js.boolean;
  > Js.t

  let make sql values is_named =
    [%bs.obj { sql; values; namedPlaceholders = is_named; }]

  let from_params sql params =
    match params with
    | None -> make sql Js.Nullable.null Js.false_
    | Some p -> match p with
      | `Named json -> make sql (Js.Nullable.return json) Js.true_
      | `Positional json -> make sql (Js.Nullable.return json) Js.false_

end

module Error = struct
  type t = {
    name:string;
    message:string;
    code:string;
    errno:int;
    sql_state:string option;
    sql_message:string option;
  }

  let default_code = "99999"
  let default_errno = 99999
  let default_message = "EMPTY_MESSAGE"
  let default_name = "UNKNOWN"

  external name : Js.Exn.t -> string Js.Nullable.t = "" [@@bs.get]
  external message : Js.Exn.t -> string Js.Nullable.t = "" [@@bs.get]
  external code : Js.Exn.t -> string Js.Nullable.t = "" [@@bs.get]
  external errno : Js.Exn.t -> int Js.Nullable.t = "" [@@bs.get]
  external sql_state : Js.Exn.t -> string Js.Nullable.t = "sqlState"
  [@@bs.get]
  external sql_message : Js.Exn.t -> string Js.Nullable.t = "sqlMessage"
  [@@bs.get]

  let make
    ~(name:string)
    ~(message:string)
    ?(code=default_code)
    ?(errno=default_errno)
    ?(sql_state:string option)
    ?(sql_message:string option)
    _ =
    { name; message; code; errno; sql_state; sql_message; }

  let null_with_default default nullable =
    match (Js.Nullable.toOption(nullable)) with
    | Some x -> x
    | None -> default


  let from_js exn = {
    name = exn |> name |> null_with_default default_name;
    message = exn |> message |> null_with_default default_message;
    code = exn |> code |> null_with_default default_code;
    errno = exn |> errno |> null_with_default default_errno;
    sql_state = exn |> sql_state |> Js.Nullable.toOption;
    sql_message = exn |> sql_message |> Js.Nullable.toOption;
  }

  let name exn = exn.name
  let message exn = exn.message
  let code exn = exn.code
  let errno exn = exn.errno
  let sql_state exn = exn.sql_state
  let sql_message exn = exn.sql_message
end

type connection = Connection.t
type meta_record = {
    catalog: string;
    schema: string;
    name: string;
    orgName: string;
    table: string;
    orgTable: string;
    characterSet: int;
    columnLength: int;
    columnType: int;
    flags: int;
    decimals: int
  }
type meta = meta_record array
type params =
  [ `Named of Js.Json.t
  | `Positional of Js.Json.t
  ] option
type rows = Js.Json.t array
type callback =
  [ `Error of Error.t
  | `Mutation of int * int
  | `Select of rows * meta
  ] ->
  unit

let decode_meta_record json = Json.Decode.({
  catalog = json |> field "catalog" string;
  schema = json |> field "schema" string;
  name = json |> field "name" string;
  orgName = json |> field "orgName" string;
  table = json |> field "table" string;
  orgTable = json |> field "orgTable" string;
  characterSet = json |> field "characterSet" int;
  columnLength = json |> field "columnLength" int;
  columnType = json |> field "columnType" int;
  flags = json |> field "flags" int;
  decimals = json |> field "decimals" int;
})

let result_mutation json = Json.Decode.(
  let changes = json |> field "affectedRows" (withDefault 0 int) in
  let last_id = json |> field "insertId" (withDefault 0 int) in
  `Mutation (changes, last_id)
)

let result_select rows meta = `Select (
  rows,
  (Belt_Array.map meta decode_meta_record)
)

let close = Connection.close

external execute :
  Connection.t ->
  Options.t ->
  (Js.Exn.t Js.Nullable.t -> Js.Json.t -> Js.Json.t array -> unit) ->
  unit = "execute"
[@@bs.send]

let parse_response json meta =
  match Js.Json.classify json with
  | Js.Json.JSONObject _ -> result_mutation json
  | Js.Json.JSONArray rows -> result_select rows meta
  | _ ->`Error (Error.make
    ~name:"UNKNOWN_RESPONSE_TYPE"
    ~message:"invalid_driver_result"
    ()
  )

let execute conn sql params callback =
  let options = Options.from_params sql params in
  execute conn options (fun exn res meta ->
    match (Js.Nullable.toOption exn) with
    | Some e -> callback (`Error (Error.from_js e))
    | None -> callback (parse_response res meta)
  )
