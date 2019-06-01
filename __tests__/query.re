open Jest;

[@bs.val] [@bs.scope "Number"]
external max_safe_integer: int = "MAX_SAFE_INTEGER";

let connect = () =>
  MySql2.Connection.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

type insert = {
  affected_rows: int,
  insert_id: option(int),
};

type thing = {
  id: int,
  code: string,
};

let raiseError = exn => exn |> raise;

let onSelect = (next, fn, res) =>
  switch (res) {
  | `Error(e) => raise(e |> MySql2.Exn.toExn)
  | `Mutation(_) => fail("unexpected_mutation_result") |> next
  | `Select(select) => fn(select, next)
  };

let onMutation = (next, fn, res) =>
  switch (res) {
  | `Error(e) => raise(e |> MySql2.Exn.toExn)
  | `Mutation(mutation) => fn(mutation, next)
  | `Select(_) => fail("unexpected_select_result") |> next
  };

describe("Raw SQL Query Test", () => {
  let conn = connect();
  afterAll(() => MySql2.Connection.close(conn));
  testAsync("Expect a test database to be listed", finish =>
    MySql2.execute(
      conn,
      "SHOW DATABASES",
      None,
      onSelect(finish, (select, next) =>
        select->(MySql2.Select.flatMap(Json.Decode.dict(Json.Decode.string)))
        |> Js.Array.map(x => Js.Dict.unsafeGet(x, "Database"))
        |> Expect.expect
        |> Expect.toContain("test")
        |> next
      ),
    )
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
    MySql2.execute(conn, "DROP TABLE IF EXISTS `test`.`simple`", None, res =>
      switch (res) {
      | `Error(e) => raiseError(e |> MySql2.Exn.toExn)
      | `Mutation(_) => next()
      | `Select(_) => failwith("unexpected_select_result")
      }
    );
  let create = next =>
    MySql2.execute(conn, table_sql, None, res =>
      switch (res) {
      | `Error(e) => raiseError(e |> MySql2.Exn.toExn)
      | `Mutation(_) => next()
      | `Select(_) => failwith("unexpected_select_result")
      }
    );
  beforeAllAsync(finish => drop(() => create(finish)));
  afterAll(() => MySql2.Connection.close(conn));
  testAsync("Expect a mutation result for an INSERT query", finish => {
    let sql = "INSERT INTO `test`.`simple` (`code`) VALUES ('foo')";
    MySql2.execute(
      conn,
      sql,
      None,
      onMutation(finish, (mutation, next) =>
        (
          MySql2.Mutation.insertId(mutation)
          ->Belt.Option.getExn
          ->MySql2.Id.toString,
          MySql2.Mutation.affectedRows(mutation),
        )
        |> Expect.expect
        |> Expect.toEqual(("1", 1))
        |> next
      ),
    );
  });
  testAsync("Expect a mutation result for an UPDATE query", finish => {
    let sql = "UPDATE `test`.`simple` SET `code`='foo2' WHERE code='foo'";
    MySql2.execute(
      conn,
      sql,
      None,
      onMutation(finish, (mutation, next) =>
        (
          MySql2.Mutation.insertId(mutation),
          MySql2.Mutation.affectedRows(mutation),
        )
        |> Expect.expect
        |> Expect.toEqual((None, 1))
        |> next
      ),
    );
  });
  testAsync("Expect a SELECT NULL to return an empty array", finish => {
    let sql = "SELECT NULL FROM `test`.`simple` WHERE false";
    let decoder = Json.Decode.dict(Json.Decode.nullable(Json.Decode.string));
    MySql2.execute(
      conn,
      sql,
      None,
      onSelect(finish, (select, next) =>
        select->(MySql2.Select.flatMap(decoder))
        |> Expect.expect
        |> Expect.toHaveLength(0)
        |> next
      ),
    );
  });
  testAsync("Expect a SELECT * to respond with all the columns", finish => {
    let sql = "SELECT * FROM `test`.`simple`";
    let decoder = json =>
      Json.Decode.{
        id: json |> field("id", int),
        code: json |> field("code", string),
      };
    let first_row = x => {
      let idIsOne = x[0].id == 1;
      let codeIsFoo = x[0].code == "foo";
      [|idIsOne, codeIsFoo|];
    };
    MySql2.execute(
      conn,
      sql,
      None,
      onSelect(finish, (select, next) =>
        select->(MySql2.Select.flatMap(decoder))
        |> first_row
        |> Expect.expect
        |> Expect.toBeSupersetOf([|true, true|])
        |> next
      ),
    );
  });
  testAsync("Expect a JS representable BIGINT be a string", finish => {
    let sql = {j|
      INSERT INTO `test`.`simple` (`id`,`code`)
      VALUES
      (9007199254740991, '2^53-1')
    |j};

    MySql2.execute(
      conn,
      sql,
      None,
      onMutation(finish, (mutation, next) =>
        MySql2.Mutation.insertId(mutation)
        ->Belt.Option.getExn
        ->MySql2.Id.toString
        |> Expect.expect
        |> Expect.toBe(max_safe_integer->string_of_int)
        |> next
      ),
    );
  });
  testAsync("Expect a non-JS representable BIGINT be a string", finish => {
    let sql = {j|
      INSERT INTO `test`.`simple` (`id`,`code`)
      VALUES
      (9007199254740993, '2^53-1')
    |j};

    MySql2.execute(
      conn,
      sql,
      None,
      onMutation(finish, (mutation, next) =>
        mutation
        ->MySql2.Mutation.insertId
        ->Belt.Option.getExn
        ->MySql2.Id.toString
        |> Expect.expect
        |> Expect.toBe("9007199254740993")
        |> next
      ),
    );
  });
});
