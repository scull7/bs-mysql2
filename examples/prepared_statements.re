let conn =
  MySql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

MySql.with_params(conn, "SELECT 1 + ? + ? as result", [|5, 6|], result =>
  switch result {
  | Error(e) => Js.log2("ERROR: ", e)
  | Mutation(m) => Js.log2("MUTATION: ", m)
  | Select(s) => Js.log2("SELECT: ", s)
  }
);

MySql.with_named_params(conn, "SELECT :x + :y as z", {"x": 1, "y": 2}, result =>
  switch result {
  | Error(e) => Js.log2("ERROR: ", e)
  | Mutation(m) => Js.log2("MUTATION: ", m)
  | Select(s) => Js.log2("SELECT: ", s)
  }
);

MySql.Connection.close(conn);
