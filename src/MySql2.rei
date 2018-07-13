module Connection: {
  type t;

  let connect:
    (
      ~host: string=?,
      ~port: int=?,
      ~user: string=?,
      ~password: string=?,
      ~database: string=?,
      unit
    ) =>
    t;

  let close: t => unit;
};

module Exn: {let fromJs: Js.Json.t => exn;};

module Id: {
  type t = MySql2_id.t;

  let fromJson: Js.Json.t => t;

  let toJson: t => Js.Json.t;

  let toString: t => string;
};

module Mutation: {
  type t;

  let insertId: t => option(MySql2_id.t);

  let fieldCount: t => int;

  let affectedRows: t => int;

  let info: t => string;

  let serverStatus: t => int;

  let warningStatus: t => int;
};

module Params: {
  type t;

  let named: Js.Json.t => t;

  let positional: Js.Json.t => t;
};

module Select: {
  type t;

  module Meta: {
    type t;

    let catalog: t => string;

    let schema: t => string;

    let name: t => string;

    let orgName: t => string;

    let table: t => string;

    let orgTable: t => string;

    let characterSet: t => int;

    let columnLength: t => int;

    let columnType: t => int;

    let flags: t => int;

    let decimals: t => int;
  };

  let meta: t => array(Meta.t);

  let concat: (t, t) => t;

  let count: t => int;

  let flatMapWithMeta: (t, (Js.Json.t, array(Meta.t)) => 'a) => array('a);

  let flatMap: (t, Js.Json.t => 'a) => array('a);

  let rows: t => array(Js.Json.t);
};

type response = [
  | `Error(exn)
  | `Mutation(Mutation.t)
  | `Select(Select.t)
];

type callback = response => unit;

let execute: (Connection.t, string, option(Params.t), callback) => unit;
