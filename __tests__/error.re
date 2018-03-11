open Jest;

let connect = () =>
  MySql2.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

describe("MySql2 Error Handling", () => {
  let conn = connect();

  afterAll(() => MySql2.close(conn));

  let accessDeniedTest = "Should respond with an access denied error";
  testAsync(accessDeniedTest, finish => {
    let c = MySql2.connect(~password="s0m3 g@rb@g3 pw", ());
    let sql = "SELECT 1+1 AS result";
    MySql2.execute(c, sql, None, res => {
      switch res {
      | `Select(_,_) => fail("unexpected_select_result") |> finish
      | `Mutation(_,_) => fail("unexpected_mutation_result") |> finish
      | `Error(e) => {
          Expect.expect(() => raise(e))
          |> Expect.toThrowMessage("ER_ACCESS_DENIED_ERROR")
          |> finish
        }
      }
    })
  });

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
