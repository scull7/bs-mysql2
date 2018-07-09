open Jest;

describe("MySql2_response", () =>
  describe("Select", () => {
    let test_meta =
      Json.Encode.(
        object_([
          ("catalog", "catalog" |. string),
          ("schema", "schema" |. string),
          ("name", "name" |. string),
          ("orgName", "orgName" |. string),
          ("table", "table" |. string),
          ("orgTable", "orgTable" |. string),
          ("characterSet", 1 |. int),
          ("columnLength", 2 |. int),
          ("columnType", 3 |. int),
          ("flags", 4 |. int),
          ("decimals", 5 |. int),
        ])
      );

    describe("count", () =>
      test("count should return 0 when rows is empty", () =>
        MySql2_response.Select.make([||], [|test_meta|])
        |. MySql2_response.Select.count
        |. Expect.expect
        |> Expect.toBe(0)
      )
    );
  })
);
