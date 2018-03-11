open Jest;

let connect = () =>
  MySql2.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

type insert = {
  affected_rows: int,
  insert_id: option(int)
};

type thing = {
  id: int,
  code: string
};

let raiseError = (exn) => exn |> raise;

let onSelect = (next, fn, res) =>
  switch res {
  | `Error(e) => raise(e);
  | `Mutation(_) => fail("unexpected_mutation_result") |> next
  | `Select(rows, meta) => fn(rows, meta, next)
};

let onMutation = (next, fn, res) =>
  switch res {
  | `Error(e) => raise(e);
  | `Mutation(count, id) => fn(count, id, next)
  | `Select(_, _) => fail("unexpected_select_result") |> next
  };

describe("Raw SQL Query Test", () => {
  let conn = connect();
  afterAll(() => MySql2.close(conn));
  testAsync("Expect a test database to be listed", finish =>
    MySql2.execute(conn, "SHOW DATABASES", None, onSelect(finish, (rows, _, next) =>
      rows
      |> Js.Array.map(Json.Decode.dict(Json.Decode.string))
      |> Js.Array.map(x => Js.Dict.unsafeGet(x, "Database"))
      |> Expect.expect
      |> Expect.toContain("test")
      |> next
    ))
  );
});

describe("Raw SQL Query Test Sequence", () => {
  let conn = connect();
  let table_sql = {|
    CREATE TABLE IF NOT EXISTS `test`.`simple` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT
    , `code` varchar(32) NOT NULL
    , PRIMARY KEY(`id`)
    )
  |};
  let drop = next =>
    MySql2.execute(conn, "DROP TABLE IF EXISTS `test`.`simple`", None, (res) =>
      switch (res) {
      | `Error(e) => raiseError(e)
      | `Mutation(_,_) => next()
      | `Select(_,_) => failwith("unexpected_select_result")
      }
    );
  let create = next =>
    MySql2.execute(conn, table_sql, None, (res) =>
      switch (res) {
      | `Error(e) => raiseError(e)
      | `Mutation(_,_) => next()
      | `Select(_,_) => failwith("unexpected_select_result")
      }
    );
  beforeAllAsync(finish => drop(() => create(finish)));
  afterAll(() => MySql2.close(conn));
  testAsync("Expect a mutation result for an INSERT query", finish => {
    let sql = "INSERT INTO `test`.`simple` (`code`) VALUES ('foo')";
    MySql2.execute(conn, sql, None, onMutation(finish, (count, id, next) => {
      let countIsOne = count == 1;
      let idIsOne = id == 1;
      Expect.expect([|countIsOne, idIsOne|])
      |> Expect.toBeSupersetOf([|true, true|])
      |> next
    }))
  });
  testAsync("Expect a mutation result for an UPDATE query", finish => {
    let sql = "UPDATE `test`.`simple` SET `code`='foo2' WHERE code='foo'";
    MySql2.execute(conn, sql, None, onMutation(finish, (count, id, next) => {
      let countIsOne = count == 1;
      let idIsZero = id == 0;
      Expect.expect([|countIsOne, idIsZero|])
      |> Expect.toBeSupersetOf([|true, true|])
      |> next
    }))
  });
  testAsync("Expect a SELECT NULL to return an empty array", finish => {
    let sql = "SELECT NULL FROM `test`.`simple` WHERE false";
    let decoder = Json.Decode.dict(Json.Decode.nullable(Json.Decode.string));
    MySql2.execute(conn, sql, None, onSelect(finish, (rows, _, next) => {
      Belt_Array.map(rows, decoder)
      |> Expect.expect
      |> Expect.toHaveLength(0)
      |> next
    }))
  });
  testAsync("Expect a SELECT * to respond with all the columns", finish =>{
    let sql = "SELECT * FROM `test`.`simple`";
    let decoder = json => Json.Decode.({
      id: json |> field("id", int),
      code: json |> field("code", string)
    });
    let first_row = x => {
      let idIsOne = x[0].id == 1;
      let codeIsFoo = x[0].code == "foo";
      [|idIsOne, codeIsFoo|];
    };
    MySql2.execute(conn, sql, None, onSelect(finish, (rows, _, next) => {
      Belt_Array.map(rows, decoder)
      |> first_row
      |> Expect.expect
      |> Expect.toBeSupersetOf([|true, true|])
      |> next
    }))
  });
});
