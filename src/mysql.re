/* Create a new Error object */
[@bs.new] external make_error : string => exn = "Error";

/* Database Exceptions */
[@bs.get] external exn_code : exn => Js.Nullable.t(string) = "code";

[@bs.get] external exn_errno : exn => Js.Nullable.t(int) = "errno";

[@bs.get] external exn_sql_state : exn => Js.Nullable.t(string) = "sqlState";

[@bs.get]
external exn_sql_message : exn => Js.Nullable.t(string) = "sqlMessage";

type connection;

type configObj;

[@bs.obj]
external makeConfigObj :
  (
    ~host: string=?,
    ~port: int=?,
    ~user: string=?,
    ~password: string=?,
    ~database: string=?,
    unit
  ) =>
  configObj =
  "";

[@bs.module "mysql2"] [@bs.val]
external createConnection : configObj => connection = "";

[@bs.send] external connection : connection => unit = "";

[@bs.send] external endConnection : connection => unit = "end";

module Connection = {
  let make = (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, ()) =>
    createConnection(
      makeConfigObj(~host?, ~port?, ~user?, ~password?, ~database?, ())
    );
  let end_ = connection => {
    endConnection(connection);
    ();
  };
};

type results('a) = Js.t('a);

type fields('a) = Js.t('a);

type placeholders('a) = Js.t('a);

[@bs.send]
external driver_query :
  (
    connection,
    string,
    (Js.Nullable.t(exn), results('a), fields('b)) => unit
  ) =>
  unit =
  "query";

[@bs.send]
external driver_execute :
  (
    connection,
    string,
    Js.Nullable.t(Js.Array.t('a)),
    (Js.Nullable.t(exn), results('b), fields('c)) => unit
  ) =>
  unit =
  "execute";

[@bs.send]
external driver_execute_named :
  (
    connection,
    Js.t('a),
    (Js.Nullable.t(exn), results('b), fields('c)) => unit
  ) =>
  unit =
  "execute";

module Execute = {
  let unnamed = (~connection, ~sql, ~placeholders=?, callback) => {
    let params = Js.Nullable.from_opt(placeholders);
    driver_execute(connection, sql, params, callback);
    ();
  };
  let named = (~connection, ~sql, ~placeholders=?, callback) => {
    let options = {
      "sql": sql,
      "values": Js.Nullable.from_opt(placeholders),
      "namedPlaceholders": Js.true_
    };
    driver_execute_named(connection, options, callback);
    ();
  };
};

let query = (connection, sql, callback) => {
  driver_query(connection, sql, callback);
  ();
};

module Promise = {
  type response('a, 'b) = {
    fields: fields('a),
    results: results('b)
  };
  let create_response =
      (results: results('a), fields: fields('b))
      : response('a, 'b) => {
    results,
    fields
  };
  let query = (connection, string) =>
    Js.Promise.make((~resolve, ~reject) =>
      driver_query(connection, string, (error, results, fields) =>
        switch (Js.Nullable.to_opt(error)) {
        | None => [@bs] resolve(create_response(results, fields))
        | Some(error) => [@bs] reject(error)
        }
      )
    );
};
