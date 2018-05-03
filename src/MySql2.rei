type params = option([ | `Named(Js.Json.t) | `Positional(Js.Json.t)]);

type rows = array(Js.Json.t);

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

module Connection: {type t;};

module Error: {let from_js: Js.Exn.t => exn;};

type connection = Connection.t;

type callback =
  [ | `Error(exn) | `Mutation(int, int) | `Select(rows, meta)] => unit;

let close: connection => unit;

let connect:
  (
    ~host: string=?,
    ~port: int=?,
    ~user: string=?,
    ~password: string=?,
    ~database: string=?,
    unit
  ) =>
  connection;

let execute: (connection, string, params, callback) => unit;

let parse_response:
  (Js.Json.t, array(Js.Json.t)) =>
  [> | `Error(exn) | `Mutation(int, int) | `Select(rows, meta)];
