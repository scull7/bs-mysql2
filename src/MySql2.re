module Connection = MySql2_connection;

module Pool = MySql2_pool;

module Exn = MySql2_error;

module Id = MySql2_id;

module Params = MySql2_params;

module Mutation = MySql2_response.Mutation;

module Select = MySql2_response.Select;

module Response = MySql2_response;

module Options = MySql2_options;

type response = [
  | `Error(Exn.t)
  | `Mutation(Mutation.t)
  | `Select(Select.t)
];

type callback = Response.t => unit;

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

let handler = (callback, exn, res, meta) =>
  (
    switch (exn |. Js.Nullable.toOption) {
    | Some(e) => `Error(e |. Exn.fromJs)
    | None => Response.fromDriverResponse(res, meta)
    }
  )
  |. callback;

let execute = (conn, sql, params, callback) => {
  let options = Options.fromParams(sql, params);

  options |. Options.namedPlaceholdersGet ?
    execute(conn, options, handler(callback)) :
    query(conn, options, handler(callback));
};
