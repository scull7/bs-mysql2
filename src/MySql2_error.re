let fromJs = (json) => {
  open Json.Decode;
  let name = json |> withDefault("UNKNOWN", field("name", string));
  let msg  = json |> withDefault("EMPTY_MESSAGE", field("message", string));
  let code = json |> withDefault("99999", field("code", string));
  let errno = json |> withDefault(99999, field("errno", int));
  let sqlState = json |> optional(field("sqlState", string));
  let sqlMessage = json |> optional(field("sqlMessage", string));

  switch((sqlState, sqlMessage)) {
  | (Some(state), Some(message)) =>
      Failure({j|$name - $code ($errno) - $msg - ($state) $message|j})
  | (Some(state), None) =>
      Failure({j|$name - $code ($errno) - $msg - ($state)|j})
  | (None, Some(message)) =>
      Failure({j|$name - $code ($errno) - $msg - $message|j})
  | (None, None) =>
      Failure({j|$name - $code ($errno) - $msg|j})
  };
};
