type t = string;

let fromJson = json => {
  Js.log2("ID JSON: ", json);
  (
    switch (json |. Js.Json.classify) {
    | Js.Json.JSONNumber(float) => float |. Js.String.make
    | Js.Json.JSONString(string) => string
    | _ => failwith("unexpected_identifier_value")
    }
  )
  |> (
    x => {
      Js.log2("PARSED ID: ", x);
      x;
    }
  );
};

let toJson = Js.Json.string;

let toString = t => t;

let isZero = t => t == "0";
