module Select = {
  /* https://mariadb.com/kb/en/library/packet_resultset/ */
  /**
  * There will be a meta data packet for each column within the
  * response.
  * @TODO - determine the meaning of the characterSet field
  * @TODO - determine the meaning of the flags field.
  */
  module Meta = {
    [@bs.deriving abstract]
    type t = {
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

    let make = json =>
      Json.Decode.(
        t(
          ~catalog=json |> field("catalog", string),
          ~schema=json |> field("schema", string),
          ~name=json |> field("name", string),
          ~orgName=json |> field("orgName", string),
          ~table=json |> field("table", string),
          ~orgTable=json |> field("orgTable", string),
          ~characterSet=json |> field("characterSet", int),
          ~columnLength=json |> field("columnLength", int),
          ~columnType=json |> field("columnType", int),
          ~flags=json |> field("flags", int),
          ~decimals=json |> field("decimals", int),
        )
      );
  };

  [@bs.deriving abstract]
  type t = {
    rows: array(Js.Json.t),
    meta: array(Meta.t),
  };

  let make = (rows, meta) =>
    t(~rows, ~meta=meta |. Belt.Array.map(Meta.make));

  let flatMap = (t, fn) =>
    Belt.Array.map(t |. rows, row => fn(row, t |. meta));

  let mapDecoder = (t, decoder) => Belt.Array.map(t |. rows, decoder);

  let count = t => Belt.Array.length(t |. rows);
};

/* https://github.com/mysqljs/mysql#getting-the-number-of-changed-rows */
/*
 * @TODO - Add changed rows
 * @TODO - determine the meaning of fieldCount
 * @TODO - determine the meaning of info
 */
module Mutation = {
  [@bs.deriving abstract]
  type t = {
    insertId: option(MySql2_id.t),
    fieldCount: int,
    affectedRows: int,
    info: string,
    serverStatus: int,
    warningStatus: int,
  };

  let parseId = json =>
    Json.Decode.(
      json
      |> field("insertId", optional(MySql2_id.fromJson))
      |. Belt.Option.flatMap(id => id |. MySql2_id.isZero ? None : Some(id))
    );

  let make = json =>
    Json.Decode.(
      t(
        ~insertId=json |> parseId,
        ~fieldCount=json |> field("fieldCount", int),
        ~affectedRows=json |> field("affectedRows", int),
        ~info=json |> field("info", string),
        ~serverStatus=json |> field("serverStatus", int),
        ~warningStatus=json |> field("warningStatus", int),
      )
    );
};

type t = [ | `Select(Select.t) | `Mutation(Mutation.t) | `Error(exn)];

let fromDriverResponse = (response, meta) =>
  switch (response |. Js.Json.classify) {
  | Js.Json.JSONArray(rows) => `Select(Select.make(rows, meta))
  | Js.Json.JSONObject(_) => `Mutation(Mutation.make(response))
  | _ => `Error(MySql2_error.invalidResponseType())
  };
