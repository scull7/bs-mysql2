let conn =
  MySql2.Connection.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let positional =
  Some(
    MySql2.Params.positional(
      Belt_Array.map([|5, 6|], Json.Encode.int) |> Json.Encode.jsonArray,
    ),
  );

MySql2.execute(conn, "SELECT 1 + ? + ? AS result", positional, res =>
  switch (res) {
  | `Error(e) => Js.log2("ERROR: ", e)
  | `Mutation(mutation) => Js.log2("Mutation: ", mutation)
  | `Select(select) => Js.log2("Select: ", select)
  }
);

let named =
  Some(
    MySql2.Params.named(
      Json.Encode.object_([
        ("x", Json.Encode.int(1)),
        ("y", Json.Encode.int(2)),
      ]),
    ),
  );

MySql2.execute(conn, "SELECT :x + :y AS result", named, res =>
  switch (res) {
  | `Error(e) => Js.log2("ERROR: ", e)
  | `Mutation(mutation) => Js.log2("Mutation: ", mutation)
  | `Select(select) => Js.log2("Select: ", select)
  }
);

MySql2.Connection.close(conn);
