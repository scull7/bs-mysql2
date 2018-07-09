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

let connect = (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, _) =>
  config(~host?, ~port?, ~user?, ~password?, ~database?, ())
  |> createConnection;
