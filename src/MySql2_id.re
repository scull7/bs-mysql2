type t = string;

let fromJson = json =>
  switch (json |. Js.Json.classify) {
  | Js.Json.JSONNumber(float) => float |. Js.Float.toString
  | Js.Json.JSONString(string) => string
  | _ => failwith("unexpected_identifier_value")
  };

let toJson = Js.Json.string;

let toString = t => t;

let isZero = t => t == "0";
