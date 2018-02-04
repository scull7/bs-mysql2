open Jest;

let connect = () =>
  MySql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

type result = {result: int};

describe("Test parameter interpolation", () => {
  let conn = connect();
  let decoder = json => {
    open! Json.Decode;
    {result: json |> field("result", int)};
  };
  describe("Standard (positional) parameters", () =>
    testAsync("Expect parameters to be substituted properly", finish =>
      MySql.Query.with_params(
        conn, "SELECT 1 + ? + ? AS result", [|5, 6|], results =>
        switch results {
        | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
        | MySql.Response.Mutation(_) =>
          fail("unexpected_mutation_result") |> finish
        | MySql.Response.Select(s) =>
          s.rows
          |> Js.Array.map(decoder)
          |> Js.Array.map(x => x.result)
          |> Expect.expect
          |> Expect.toBeSupersetOf([|12|])
          |> finish
        }
      )
    )
  );
  describe("Named parameters", () =>
    testAsync("Expect parameters to be substitude properly", finish =>
      MySql.Query.with_named_params(
        conn, "SELECT :x + :y AS result", {"x": 1, "y": 2}, results =>
        switch results {
        | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
        | MySql.Response.Mutation(_) =>
          fail("unexpected_mutation_result") |> finish
        | MySql.Response.Select(s) =>
          s.rows
          |> Js.Array.map(decoder)
          |> Js.Array.map(x => x.result)
          |> Expect.expect
          |> Expect.toBeSupersetOf([|3|])
          |> finish
        }
      )
    )
  );
  afterAll(() => MySql.Connection.close(conn));
});
