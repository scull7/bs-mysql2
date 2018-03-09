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
      | `Anonymous json -> make sql (Js.Nullable.return json) Js.false_

end

type t
type callback = Js.Exn.t Js.Nullable.t -> Js.Json.t -> Js.Json.t array -> unit
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
  | `Anonymous of Js.Json.t
  ] option
type rows = Js.Json.t array

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

external execute : Connection.t -> 'a Js.t -> callback -> unit = "execute"
[@@bs.send]

let execute conn sql params callback =
  let options = Options.from_params sql params in
  execute conn options callback

let parse_response json meta =
  match Js.Json.classify json with
  | Js.Json.JSONObject _ -> result_mutation json
  | Js.Json.JSONArray rows -> result_select rows meta
  | _ -> failwith "invalid_driver_result"
