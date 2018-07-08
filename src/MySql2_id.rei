type t;

let fromJson: Js.Json.t => t;

let toJson: t => Js.Json.t;

let toString: t => string;

let isZero: t => bool;
