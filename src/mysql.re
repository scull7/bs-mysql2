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

let createConnection =
    (~host=?, ~port=?, ~user=?, ~password=?, ~database=?, ()) =>
  createConnection(
    makeConfigObj(~host?, ~port?, ~user?, ~password?, ~database?, ())
  );

type error =
  Js.Nullable.t(
    {
      .
      "code": string,
      "fatal": bool,
      "message": string
    }
  );

type results;

type fields;

[@bs.send]
external query : (connection, string, (error, results, fields) => unit) => unit =
  "";

[@bs.send] external connection : connection => unit = "";

[@bs.send] external endConnection : connection => unit = "end";
