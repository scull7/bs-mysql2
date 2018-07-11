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

    describe("concat", () =>
      test("should combine the rows of two result sets", () => {
        let rows1 = [|Js.Json.string("foo"), Js.Json.string("bar")|];
        let rows2 = [|Js.Json.string("boo"), Js.Json.string("baz")|];

        let t1 = MySql2_response.Select.make(rows1, [|test_meta|]);
        let t2 = MySql2_response.Select.make(rows2, [|test_meta|]);

        MySql2_response.Select.concat(t1, t2)
        |. MySql2_response.Select.mapDecoder(x =>
             x |. Js.Json.decodeString |. Belt.Option.getExn
           )
        |. Expect.expect
        |> Expect.toEqual([|"foo", "bar", "boo", "baz"|]);
      })
    );

    describe("count", () =>
      test("count should return 0 when rows is empty", () =>
        MySql2_response.Select.make([||], [|test_meta|])
        |. MySql2_response.Select.count
        |. Expect.expect
        |> Expect.toBe(0)
      )
    );

    describe("rows", () =>
      test("rows should return the current set of rows", () => {
        let rows = [|Js.Json.string("foo"), Js.Json.string("bar")|];

        MySql2_response.Select.make(rows, [|test_meta|])
        |. MySql2_response.Select.rows
        |. Belt.Array.map(x =>
             x |. Js.Json.decodeString |. Belt.Option.getExn
           )
        |. Expect.expect
        |> Expect.toEqual([|"foo", "bar"|]);
      })
    );
  })
);
