open Jest;

let connect = () =>
  MySql2.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ~password="", ~database="test", ());

type result = {result: int};

let message = (exn) =>
  switch (Js.Exn.message(exn)) {
  | Some(message) => message
  | None => switch (Js.Exn.name(exn)) {
      | Some(name) => name
      | None => "Unknown Error"
      }
  }
;

describe("Test parameter interpolation", () => {
  let conn = connect();
  let decoder = json => Json.Decode.({
    result: json |> field("result", int)
  });
  afterAll(() => MySql2.Connection.close(conn));
  describe("Standard (positional) parameters", () => {
    testAsync("Expect parameters to be substituted properly", finish => {
      let sql = "SELECT 1 + ? + ? AS result";
      let params = Some(`Anonymous(
        Belt_Array.map([|5,6|], Json.Encode.int) |> Json.Encode.jsonArray
      ));
      MySql2.execute(conn, sql, params, (exn, res, meta) =>
        switch (Js.Nullable.toOption(exn)) {
        | Some(e) => message(e) |> fail |> finish
        | None =>
          switch (MySql2.parse_response(res, meta)) {
          | `Error(e) => message(e) |> fail |> finish
          | `Mutation(_,_) => fail("unexpected_mutation_result") |> finish
          | `Select(rows, _) =>
              Belt_Array.map(rows, decoder)
              |> Belt_Array.map(_, x => x.result)
              |> Expect.expect
              |> Expect.toBeSupersetOf([|12|])
              |> finish
          }
        }
      )
    });
  });
  describe("Named parameters", () => {
    testAsync("Expect parameters to be substituted properly", finish => {
      let sql = "SELECT :x + :y AS result";
      let params = Some(`Named(
        Json.Encode.object_([
          ("x", Json.Encode.int(1)),
          ("y", Json.Encode.int(2)),
        ])
      ));
      MySql2.execute(conn, sql, params, (exn, res, meta) =>
        switch(Js.Nullable.toOption(exn)) {
        | Some(e) => message(e) |> fail |> finish
        | None =>
          switch (MySql2.parse_response(res, meta)) {
          | `Error(e) => message(e) |> fail |> finish
          | `Mutation(_,_) => fail("unexpected_mutation_result") |> finish
          | `Select(rows, _) =>
              Belt_Array.map(rows, decoder)
              |> Belt_Array.map(_, x => x.result)
              |> Expect.expect
              |> Expect.toBeSupersetOf([|3|])
              |> finish
          }
        }
      );
    });
  });
});
