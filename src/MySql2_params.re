type t = [ | `Named(Js.Json.t) | `Positional(Js.Json.t)];

let named = json => `Named(json);

let positional = json => `Positional(json);
