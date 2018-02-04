/*
   @TODO - write some sort of function that differentiates between a Select and Mutation response.
     Want to allow the user to say, "I expect a Select response and anything else is an Error."
 */
open Jest;

let connect = () =>
  MySql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

type insert = {
  affected_rows: int,
  insert_id: option(int)
};

type thing = {
  id: int,
  code: string
};

describe("Raw SQL Query Test", () => {
  let conn = connect();
  afterAll(() => MySql.Connection.close(conn));
  testAsync("Expect a test database to be listed", finish =>
    MySql.Query.raw(conn, "SHOW DATABASES", results =>
      switch results {
      | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
      | MySql.Response.Mutation(_) => fail("Got mutation result") |> finish
      | MySql.Response.Select(s) =>
        s.rows
        |> Js.Array.map(Json.Decode.dict(Json.Decode.string))
        |> Js.Array.map(x => Js.Dict.unsafeGet(x, "Database"))
        |> Expect.expect
        |> Expect.toContain("test")
        |> finish
      }
    )
  );
});

describe("Raw SQL Query Test Sequence", () => {
  let conn = connect();
  let table_sql = {|
    CREATE TABLE IF NOT EXISTS test.simple (
      id bigint(20) NOT NULL AUTO_INCREMENT
    , code varchar(32) NOT NULL
    , PRIMARY KEY(id)
    )
  |};
  let drop = next =>
    MySql.Query.raw(conn, "DROP TABLE IF EXISTS test.simple", response =>
      switch response {
      | MySql.Response.Error(e) => raise(e)
      | MySql.Response.Select(_) => failwith("unexpected_select_result")
      | MySql.Response.Mutation(_) => next()
      }
    );
  let create = next =>
    MySql.Query.raw(conn, table_sql, response =>
      switch response {
      | MySql.Response.Error(e) => raise(e)
      | MySql.Response.Select(_) => failwith("unexpected_select_result")
      | MySql.Response.Mutation(_) => next()
      }
    );
  beforeAllAsync(finish => drop(() => create(finish)));
  testAsync("Expect a mutation result for an INSERT query", finish =>
    MySql.Query.raw(
      conn, "INSERT INTO test.simple (code) VALUES ('foo')", results =>
      switch results {
      | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
      | MySql.Response.Select(_) => fail("unexpected_select_result") |> finish
      | MySql.Response.Mutation(m) =>
        let affectedRows = m.affected_rows == 1;
        let insertId =
          switch m.insert_id {
          | Some(_) => true
          | None => false
          };
        [|affectedRows, insertId|]
        |> Expect.expect
        |> Expect.toBeSupersetOf([|true, true|])
        |> finish;
      }
    )
  );
  testAsync("Expect a mutation result for an UPDATE query", finish =>
    MySql.Query.raw(
      conn, "UPDATE test.simple SET code='foo2' WHERE code='foo'", results =>
      switch results {
      | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
      | MySql.Response.Select(_) => fail("unexpected_select_result") |> finish
      | MySql.Response.Mutation(m) =>
        let affectedRows = m.affected_rows == 1;
        let insertId =
          switch m.insert_id {
          | Some(_) => false
          | None => true
          };
        [|affectedRows, insertId|]
        |> Js.Array.reduce((x, acc) => x && acc, true)
        |> Expect.expect
        |> Expect.toBe(true)
        |> finish;
      }
    )
  );
  testAsync("Expect a SELECT NULL to return an empty array", finish =>
    MySql.Query.raw(conn, "SELECT NULL FROM test.simple WHERE false", results =>
      switch results {
      | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
      | MySql.Response.Mutation(_) =>
        fail("unexpected_mutation_result") |> finish
      | MySql.Response.Select(s) =>
        s.rows
        |> Js.Array.map(
             Json.Decode.dict(Json.Decode.nullable(Json.Decode.string))
           )
        |> Expect.expect
        |> Expect.toHaveLength(0)
        |> finish
      }
    )
  );
  let decoder = json => {
    open! Json.Decode;
    {id: json |> field("id", int), code: json |> field("code", string)};
  };
  testAsync("Expect a SELECT * to respond with all the columns", finish =>
    MySql.Query.raw(conn, "SELECT * FROM test.simple", results =>
      switch results {
      | MySql.Response.Error(_) => fail("Unexpected exception") |> finish
      | MySql.Response.Mutation(_) =>
        fail("unexpected_mutation_result") |> finish
      | MySql.Response.Select(s) =>
        s.rows
        |> Js.Array.map(decoder)
        |> (
          x =>
            {
              let id = x[0].id == 1;
              let code = x[0].code == "foo";
              [|id, code|];
            }
            |> Expect.expect
            |> Expect.toBeSupersetOf([|true, true|])
            |> finish
        )
      }
    )
  );
  afterAll(() => MySql.Connection.close(conn));
});
