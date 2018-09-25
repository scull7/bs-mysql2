type t = {
  name: string,
  msg: string,
  code: string,
  errno: int,
  sql: option(string),
  sqlState: option(string),
  sqlMessage: option(string),
};

let make =
    (~name, ~msg, ~code, ~errno, ~sql=?, ~sqlState=?, ~sqlMessage=?, ()) => {
  name,
  msg,
  code,
  errno,
  sql,
  sqlState,
  sqlMessage,
};

let fromJs = json =>
  Json.Decode.{
    name: json |> withDefault("UNKNOWN", field("name", string)),
    msg: json |> withDefault("EMPTY_MESSAGE", field("message", string)),
    code: json |> withDefault("99999", field("code", string)),
    errno: json |> withDefault(99999, field("errno", int)),
    sql: json |> optional(field("sql", string)),
    sqlState: json |> optional(field("sqlState", string)),
    sqlMessage: json |> optional(field("sqlMessage", string)),
  };

let toExn = t => {
  let {name, code, errno, msg, sqlState, sqlMessage, _} = t;

  switch (sqlState, sqlMessage) {
  | (Some(state), Some(message)) =>
    Failure({j|$name - $code ($errno) - $msg - ($state) $message|j})
  | (Some(state), None) =>
    Failure({j|$name - $code ($errno) - $msg - ($state)|j})
  | (None, Some(message)) =>
    Failure({j|$name - $code ($errno) - $msg - $message|j})
  | (None, None) => Failure({j|$name - $code ($errno) - $msg|j})
  };
};

let fromJsToExn = json => json |> fromJs |> toExn;

let invalidResponseType = () =>
  make(
    ~name="InvalidResponseType",
    ~msg="invalid_driver_result",
    ~code="UNKNOWN_RESPONSE_TYPE",
    ~errno=99999,
    (),
  );
