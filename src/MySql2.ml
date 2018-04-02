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

  let with_default default nullable =
    nullable
    |> Js.Nullable.toOption
    |> Js.Option.getWithDefault default

  let from_js exn =
    let name = exn |> name |> with_default default_name in
    let message = exn |> message |> with_default default_message in
    let code = exn |> code |> with_default default_code in
    let errno = exn |> errno |> with_default default_errno in
    let sql_state = exn |> sql_state in
    let sql_message = exn |> sql_message in
    Failure {j|$name - $code ($errno) - $message - ($sql_state) $sql_message|j}
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
  [ `Error of exn
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
let connect = Connection.make

external execute :
  Connection.t ->
  Options.t ->
  (Js.Exn.t Js.Nullable.t -> Js.Json.t -> Js.Json.t array -> unit) ->
  unit = "execute"
[@@bs.send]

external query :
  Connection.t ->
  Options.t ->
  (Js.Exn.t Js.Nullable.t -> Js.Json.t -> Js.Json.t array -> unit) ->
  unit = "query"
[@@bs.send]

let parse_response json meta =
  match Js.Json.classify json with
  | Js.Json.JSONObject _ -> result_mutation json
  | Js.Json.JSONArray rows -> result_select rows meta
  | _ -> `Error (Failure
      {|MySql2Error - (UNKNOWN_RESPONSE_TYPE) - invalid_driver_result|}
    )

let execute conn sql params callback =
  let options = Options.from_params sql params in
  let fn = if options##namedPlaceholders == Js.true_
    then execute
    else query
  in
  fn conn options (fun exn res meta ->
    match (Js.Nullable.toOption exn) with
    | Some e -> callback (`Error (Error.from_js e))
    | None -> callback (parse_response res meta)
  )
