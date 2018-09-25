[@bs.deriving abstract]
type t = {
  [@bs.optional]
  mutable failed: bool,
};

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

[@bs.send]
external on :
  (t, [@bs.string] [ | `error([@bs.this] ((t, MySql2_error.t) => unit))]) => t =
  "";

[@bs.module "mysql2"] external createConnection : config => t = "";
[@bs.send] external close : t => unit = "end";

let isValid = connection =>
  failedGet(connection) |> Belt.Option.getWithDefault(_, true);

/**
  For now we just consider all connection level errors to be
  irrecoverable, therefore, we invalidate the connection.
*/
let errorHandler =
  [@bs.this]
  (
    (connection, error) => {
      Js.Console.error2("MYSQL2 - CONNECTION - FAILURE", error);
      failedSet(connection, true);
      ();
    }
  );

let connect = (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, _) =>
  config(~host?, ~port?, ~user?, ~password?, ~database?, ())
  |> createConnection
  |> on(_, `error(errorHandler));
