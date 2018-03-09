
let conn =
  MySql2.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

let positional = Some(`Anonymous(
  Belt_Array.map([|5,6|], Json.Encode.int) |> Json.Encode.jsonArray
));

MySql2.execute(conn, "SELECT 1 + ? + ? AS result", positional, (exn, res, meta) =>
  switch (Js.Nullable.toOption(exn)) {
  | Some(e) => Js.log2("ERROR: ", e)
  | None =>
    switch (MySql2.parse_response(res, meta)) {
    | `Error(e) => Js.log2("ERROR: ", e)
    | `Mutation(count, id) => Js.log3("Mutation: ", count, id)
    | `Select(rows, meta) => Js.log3("Select: ", rows, meta)
    }
  }
);

let named = Some(`Named(
  Json.Encode.object_([
    ("x", Json.Encode.int(1)),
    ("y", Json.Encode.int(2)),
  ])
));
MySql2.execute(conn, "SELECT :x + :y AS result", named, (exn, res, meta) =>
  switch (Js.Nullable.toOption(exn)) {
  | Some(e) => Js.log2("ERROR: ", e)
  | None =>
    switch (MySql2.parse_response(res, meta)) {
    | `Error(e) => Js.log2("ERROR: ", e)
    | `Mutation(count, id) => Js.log3("Mutation: ", count, id)
    | `Select(rows, meta) => Js.log3("Select: ", rows, meta)
    }
  }
);

MySql2.close(conn);
