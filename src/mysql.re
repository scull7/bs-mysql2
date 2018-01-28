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

type error = Js.Nullable.t(exn);

type results('a) = Js.t('a);

type fields('a) = Js.t('a);

type unnamed('a) = option(Js.Array.t('a));

type named('a) = option(Js.t('a));

[@bs.send]
external driver_query :
  (connection, string, (error, results('a), fields('b)) => unit) => unit =
  "query";

[@bs.send]
external driver_execute :
  (
    connection,
    string,
    Js.Nullable.t(Js.Array.t('a)),
    (error, results('b), fields('c)) => unit
  ) =>
  unit =
  "execute";

[@bs.send]
external driver_execute_named :
  (connection, Js.t('a), (error, results('b), fields('c)) => unit) => unit =
  "execute";

module Execute = {
  let unnamed =
      (
        ~connection: connection,
        ~sql: string,
        ~placeholders: unnamed('a)=?,
        callback
      ) => {
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
  module Connection = {
    let end_ = (connection, x) =>
      Js.Promise.resolve(x)
      |> Js.Promise.then_(x => {
           Connection.end_(connection);
           x;
         });
  };
  let handler = (resolve, reject, error: error, rows, fields) =>
    switch (Js.Nullable.to_opt(error)) {
    | Some(e) => [@bs] reject(e)
    | None => [@bs] resolve((rows, fields))
    };
  let query = (~connection, ~sql, ~placeholders=?, _) =>
    Js.Promise.make((~resolve, ~reject) =>
      Execute.unnamed(
        ~connection,
        ~sql,
        ~placeholders?,
        handler(resolve, reject)
      )
    );
  let pquery = (~sql, ~placeholders=?, pconn: Js.Promise.t(connection)) =>
    pconn
    |> Js.Promise.then_(conn =>
         query(~connection=conn, ~sql, ~placeholders?, ())
       );
};
