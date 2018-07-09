[@bs.deriving abstract]
type t = {
  sql: string,
  values: Js.Nullable.t(Js.Json.t),
  namedPlaceholders: bool,
};

let fromParams = (sql, params) =>
  switch (params) {
  | None => t(~sql, ~values=Js.Nullable.null, ~namedPlaceholders=false)
  | Some(p) =>
    switch (p) {
    | `Named(json) =>
      t(~sql, ~values=Js.Nullable.return(json), ~namedPlaceholders=true)
    | `Positional(json) =>
      t(~sql, ~values=Js.Nullable.return(json), ~namedPlaceholders=false)
    }
  };
