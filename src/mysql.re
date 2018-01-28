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

type mysql_results('a) = Js.t('a);

type mysql_fields('a) = Js.t('a);

type mysql_response('a, 'b) = {
  fields: mysql_fields('a),
  results: mysql_results('b)
};

let create_response =
    (results: mysql_results('a), fields: mysql_fields('b))
    : mysql_response('a, 'b) => {
  results,
  fields
};

[@bs.send]
external query :
  (
    connection,
    string,
    (Js.Nullable.t(exn), mysql_results('a), mysql_fields('b)) => unit
  ) =>
  unit =
  "";

module Promise = {
  let query = (connection, string) =>
    Js.Promise.make((~resolve, ~reject) =>
      query(connection, string, (error, results, fields) =>
        switch (Js.Nullable.to_opt(error)) {
        | None => [@bs] resolve(create_response(results, fields))
        | Some(error) => [@bs] reject(error)
        }
      )
    );
};
