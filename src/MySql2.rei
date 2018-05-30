type params = option([ | `Named(Js.Json.t) | `Positional(Js.Json.t)]);
type rows = array(Js.Json.t);
type metaRecord;
type meta = array(metaRecord);
type connection;


module Error: {
  let fromJs: Js.Json.t => exn;
};

type response = [
  | `Error(exn)
  | `Mutation(int, int)
  | `Select(rows, meta)
];

type callback = response => unit;

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

let parseResponse: (Js.Json.t, array(Js.Json.t)) => response;
