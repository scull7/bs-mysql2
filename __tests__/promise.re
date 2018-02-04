open Jest;

let connect = () =>
  MySql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

type result = {search: string};

describe("Test Promise based API", () => {
  let conn = connect();
  let decoder = json => {
    open! Json.Decode;
    {search: json |> field("search", string)};
  };
  testPromise("Simple string interpolation query", () =>
    Js.Promise.resolve(conn)
    |> MySql.Promise.pipe_with_params("SELECT ? AS search", [|"%schema"|])
    |> Js.Promise.then_(results =>
         switch results {
         | MySql.Promise.Mutation(_) => failwith("unexpected_mutation_result")
         | MySql.Promise.Select(s) =>
           s.rows
           |> Js.Array.map(decoder)
           |> Js.Array.map(x => x.search)
           |> Expect.expect
           |> Expect.toBeSupersetOf([|"%schema"|])
           |> Js.Promise.resolve
         }
       )
  );
});
