open Jest;

let connect = () =>
  MySql2.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

describe("MySql2 Error Handling", () => {
  let conn = connect();

  afterAll(() => MySql2.close(conn));

  let syntaxErrorTest = "Should respond with an error on invalid SQL syntax.";
  testAsync(syntaxErrorTest, finish => {
    let sql = "SELECT invalid, AS result";
    MySql2.execute(conn, sql, None, res => {
      switch res {
      | `Select(_,_) => fail("unexpected_select_result") |> finish
      | `Mutation(_,_) => fail("unexpected_mutation_result") |> finish
      | `Error(e) => {
          MySql2.Error.code(e)
          |> Expect.expect
          |> Expect.toBe("ER_PARSE_ERROR")
          |> finish
        }
      }
    })
  });

  let errorTestGen = (name, testfn) =>
    testAsync({j|Should return an error with a $name field|j}, finish => {
      let sql = "SELECT invalid, AS result";
      MySql2.execute(conn, sql, None, res => {
        switch res {
        | `Select(_,_) => fail("unexpected_select_result") |> finish
        | `Mutation(_,_) => fail("unexpected_mutation_result") |> finish
        | `Error(e) => testfn(finish, e)
        }
      })
    });

  errorTestGen("name", (finish, e) => {
    MySql2.Error.name(e)
    |> Expect.expect
    |> Expect.toBe("Error")
    |> finish
  });

  errorTestGen("message", (finish, e) => {
    MySql2.Error.message(e)
    |> Expect.expect
    |> Expect.toMatch("SQL syntax")
    |> finish
  });

  errorTestGen("code", (finish, e) => {
    MySql2.Error.code(e)
    |> Expect.expect
    |> Expect.toBe("ER_PARSE_ERROR")
    |> finish
  });

  errorTestGen("errno", (finish, e) => {
    MySql2.Error.errno(e)
    |> Expect.expect
    |> Expect.toBe(1064)
    |> finish
  });

  errorTestGen("sqlState", (finish, e) => {
    MySql2.Error.sql_state(e)
    |> Js.Option.getWithDefault("Unexpected NULL")
    |> Expect.expect
    |> Expect.toBe("42000")
    |> finish
  });

  errorTestGen("sqlMessage", (finish, e) => {
    MySql2.Error.sql_message(e)
    |> Js.Option.getWithDefault("Unexpected NULL")
    |> Expect.expect
    |> Expect.toMatch("SQL syntax")
    |> finish
  });
});
