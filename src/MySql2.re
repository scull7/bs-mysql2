module Connection = {
  type t;

  module Config = {
    type t;

    [@bs.obj] external make : (~host: string=?, ~port: int=?, ~user: string=?, ~password: string=?, ~database: string=?, unit) => t = "";
  };

  [@bs.module "mysql2"] external createConnection : Config.t => t = "";
  [@bs.send] external close : t => unit = "end";

  let make (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, _) =
    createConnection(Config.make(~host=?host, ~port=?port, ~user=?user, ~password=?password, ~database=?database, ()));
};

module Options = {
  type t = Js.t({
    .
    sql: string,
    values: Js.Nullable.t(Js.Json.t),
    namedPlaceholders: bool,
   });

  let make = (sql, values, is_named) =>
    [%bs.obj {
      sql,
      values,
      namedPlaceholders: is_named,
    }];

  let from_params(sql) =
    fun
    | None => make(sql, Js.Nullable.null, false)
    | Some(p) => p |>
      fun
      | `Named(json) => make(sql, Js.Nullable.return(json), true)
      | `Positional(json) => make(sql, Js.Nullable.return(json), false)
};

module Error = {
  let default_code = "99999";
  let default_errno = 99999;
  let default_message = "EMPTY_MESSAGE";
  let default_name = "UNKNOWN";

  [@bs.get] external name : Js.Exn.t => Js.Nullable.t(string) = "";
  [@bs.get] external message : Js.Exn.t => Js.Nullable.t(string) = "";
  [@bs.get] external code : Js.Exn.t => Js.Nullable.t(string) = "";
  [@bs.get] external errno : Js.Exn.t => Js.Nullable.t(int) = "";
  [@bs.get] external sql_state : Js.Exn.t => Js.Nullable.t(string) = "sqlState";
  [@bs.get] external sql_message : Js.Exn.t => Js.Nullable.t(string) = "sqlMessage";

  let with_default = (default, nullable) =>
    nullable
    |> Js.Nullable.toOption
    |> Js.Option.getWithDefault(default);

  let from_js = exn => {
    let name = exn |> name |> with_default(default_name);
    let message = exn |> message |> with_default(default_message);
    let code = exn |> code |> with_default(default_code);
    let errno = exn |> errno |> with_default(default_errno);
    let sql_state = exn |> sql_state;
    let sql_message = exn |> sql_message;
    Failure({j|$name - $code ($errno) - $message - ($sql_state) $sql_message|j})
  };
};

type connection = Connection.t;
type meta_record = {
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
type meta = array(meta_record);
type params = option([
  | `Named(Js.Json.t)
  | `Positional(Js.Json.t)
  ]);
type rows = array(Js.Json.t);
type callback = [
  | `Error(exn)
  | `Mutation(int, int)
  | `Select(rows, meta)
  ] => unit;

let decode_meta_record = json =>
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

let result_mutation = json => {
  open Json.Decode;
  let changes = json |> field("affectedRows", withDefault(0, int));
  let last_id = json |> field("insertId",withDefault(0, int));
  `Mutation((changes, last_id));
};

let result_select = (rows, meta) =>
  `Select(
  rows,
  Belt_Array.map(meta, decode_meta_record)
);

let close = Connection.close;
let connect = Connection.make;

[@bs.send] external execute :
  (
    Connection.t,
    Options.t,
    (
      Js.Nullable.t(Js.Exn.t),
      Js.Json.t,
      array(Js.Json.t)
    ) => unit
  ) => unit = "execute";

[@bs.send] external query :
  (
    Connection.t,
    Options.t,
    (
      Js.Nullable.t(Js.Exn.t),
      Js.Json.t,
      array(Js.Json.t)
    ) => unit
  ) => unit = "query";

let parse_response = (json, meta) =>
  switch(json |> Js.Json.classify) {
  | Js.Json.JSONObject(_) => result_mutation(json)
  | Js.Json.JSONArray(rows) => result_select(rows, meta)
  | _ => `Error (
    Failure({|MySql2Error - (UNKNOWN_RESPONSE_TYPE) - invalid_driver_result|}),
  )
  };

let execute = (conn, sql, params, callback) => {
  let options = params |> Options.from_params(sql);
  let fn =
    if (options##namedPlaceholders) {
      execute
    } else {
      query
    };
  
  fn(conn, options, (exn, res, meta) =>
    switch (exn |> Js.Nullable.toOption) {
    | Some(e) => callback (`Error(e |> Error.from_js))
    | None => callback(meta |> parse_response(res))
    }
  );
};
