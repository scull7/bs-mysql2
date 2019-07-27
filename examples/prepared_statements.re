let conn =
  MySql2.Connection.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let positional =
  Some(
    Belt_Array.map([|5, 6|], float_of_int)
    |> Js.Json.numberArray
    |> MySql2.Params.positional,
  );

MySql2.execute(conn, "SELECT 1 + ? + ? AS result", positional, res =>
  switch (res) {
  | `Error(e) => Js.log2("ERROR: ", e)
  | `Mutation(mutation) => Js.log2("Mutation: ", mutation)
  | `Select(select) => Js.log2("Select: ", select)
  }
);

let obj = Js.Dict.empty();
obj->Js.Dict.set("x", float_of_int(1)->Js.Json.number);
obj->Js.Dict.set("y", float_of_int(2)->Js.Json.number);

let named = Some(obj->Js.Json.object_->MySql2.Params.named);

MySql2.execute(conn, "SELECT :x + :y AS result", named, res =>
  switch (res) {
  | `Error(e) => Js.log2("ERROR: ", e)
  | `Mutation(mutation) => Js.log2("Mutation: ", mutation)
  | `Select(select) => Js.log2("Select: ", select)
  }
);

MySql2.Connection.close(conn);
