open Jest;

/* This will cover the connection which uses only default options */
let connect = () => MySql2.Connection.make(());

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
          Expect.expect(() => raise(e))
          |> Expect.toThrowMessage("ER_PARSE_ERROR")
          |> finish
        }
      }
    })
  });

  let emptyErrorTest = "Should parse out an empty error with defaults";
  test(emptyErrorTest, () => {
    try (
      Js.Exn.raiseError("IDKWTM")
    ) {
    | Js.Exn.Error(e) => {
        let exn = MySql2.Error.from_js(e);
        Expect.expect(() => raise(exn))
        |> Expect.toThrowMessage("99999 (99999) - IDKWTM")
      }
    }
  });

  let nonErrorObjectTest = "Should return a defaulted error";
  test(nonErrorObjectTest, () => {
    try (
      /* Use raw JS here to throw a garbage exception object */
      [%raw {|(function () { throw {} })()|}]
    ) {
    | Js.Exn.Error(e) => {
        let exn = MySql2.Error.from_js(e);
        Expect.expect(()=> raise(exn))
        |> Expect.toThrowMessage("UNKNOWN - 99999 (99999) - EMPTY_MESSAGE")
      }
    }
  });
});
