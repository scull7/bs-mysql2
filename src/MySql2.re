module Error = MySql2_error;

module Connection = {
  type t;

  [@bs.deriving abstract]
  type config = {
    [@bs.optional]
    host: string,
    [@bs.optional]
    port: int,
    [@bs.optional]
    user: string,
    [@bs.optional]
    password: string,
    [@bs.optional]
    database: string,
  };

  [@bs.module "mysql2"] external createConnection : config => t = "";
  [@bs.send] external close : t => unit = "end";

  let make = (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, _) =>
    config(~host?, ~port?, ~user?, ~password?, ~database?, ())
    |> createConnection;
};

module Options = {
  [@bs.deriving abstract]
  type t = {
    sql: string,
    values: Js.Nullable.t(Js.Json.t),
    namedPlaceholders: bool,
  };

  let fromParams = (sql, params) =>
    switch (params) {
    | None => t(~sql, ~values=Js.Nullable.null, ~namedPlaceholders=false)
    | Some(p) =>
      switch (p) {
      | `Named(json) =>
        t(~sql, ~values=Js.Nullable.return(json), ~namedPlaceholders=true)
      | `Positional(json) =>
        t(~sql, ~values=Js.Nullable.return(json), ~namedPlaceholders=false)
      }
    };
};

type connection = Connection.t;
type metaRecord = {
  catalog: string,
  schema: string,
  name: string,
  orgName: string,
  table: string,
  orgTable: string,
  characterSet: int,
  columnLength: int,
  columnType: int,
  flags: int,
  decimals: int,
};
type meta = array(metaRecord);
type params = option([ | `Named(Js.Json.t) | `Positional(Js.Json.t)]);
type rows = array(Js.Json.t);
type response = [
  | `Error(exn)
  | `Mutation(int, int)
  | `Select(rows, meta)
];
type callback = response => unit;

let decodeMetaRecord = json =>
  Json.Decode.{
    catalog: json |> field("catalog", string),
    schema: json |> field("schema", string),
    name: json |> field("name", string),
    orgName: json |> field("orgName", string),
    table: json |> field("table", string),
    orgTable: json |> field("orgTable", string),
    characterSet: json |> field("characterSet", int),
    columnLength: json |> field("columnLength", int),
    columnType: json |> field("columnType", int),
    flags: json |> field("flags", int),
    decimals: json |> field("decimals", int),
  };

let decodeResultMutation = json => {
  open Json.Decode;
  let changes = json |> field("affectedRows", withDefault(0, int));
  let last_id = json |> field("insertId", withDefault(0, int));

  `Mutation((changes, last_id));
};

let decodeResultSelect = (rows, meta) =>
  `Select((rows, Belt.Array.map(meta, decodeMetaRecord)));

let close = Connection.close;
let connect = Connection.make;

[@bs.send]
external execute :
  (
    Connection.t,
    Options.t,
    (Js.Nullable.t(Js.Json.t), Js.Json.t, array(Js.Json.t)) => unit
  ) =>
  unit =
  "execute";

[@bs.send]
external query :
  (
    Connection.t,
    Options.t,
    (Js.Nullable.t(Js.Json.t), Js.Json.t, array(Js.Json.t)) => unit
  ) =>
  unit =
  "query";

let parseResponse = (json, meta) =>
  switch (json |> Js.Json.classify) {
  | Js.Json.JSONObject(_) => decodeResultMutation(json)
  | Js.Json.JSONArray(rows) => decodeResultSelect(rows, meta)
  | _ =>
    `Error(
      Failure(
        {| MySql2Error - (UNKNOWN_RESPONSE_TYPE) - invalid_driver_result|},
      ),
    )
  };

let execute = (conn, sql, params, callback) => {
  let options = Options.fromParams(sql, params);
  let fn =
    if (Options.namedPlaceholders(options)) {
      execute;
    } else {
      query;
    };

  fn(
    conn,
    options,
    (exn, res, meta) => {
      let response =
        switch (Js.Nullable.toOption(exn)) {
        | Some(e) => `Error(Error.fromJs(e))
        | None => parseResponse(res, meta)
        };
      callback(response);
    },
  );
};
