open Jest;

let connect = () =>
  MySql2.Pool.make(
    ~connectionLimit=2,
    ~queueLimit=0,
    ~waitForConnections=true,
    ~host="127.0.0.1",
    ~port=3306,
    ~user="root",
    ~password="",
    ~database="test",
    (),
  );

let connectWithDefaults = () => MySql2.Pool.make();

let select = (db, sql, params, cb) => {
  let returnError = e =>
    e->Js.Json.stringifyAny->Belt.Option.getExn->Belt.Result.Error->cb;

  let gotMutation = weird => {
    let result = weird->Js.Json.stringifyAny->Belt.Option.getExn;
    {j|Expected select, go mutation :: $result|j}->Belt.Result.Error->cb;
  };

  MySql2.execute(db, sql, params, response =>
    switch (response) {
    | `Error(e) => returnError(e)
    | `Mutation(weird) => gotMutation(weird)
    | `Select(result) => result->Belt.Result.Ok->cb
    }
  );
};

describe("MySql2.Pool", () => {
  let getDb = (poolFn, callback) => {
    let pool = poolFn();

    MySql2.Pool.getConnection(pool, (error, db) =>
      switch (
        error->Js.Null_undefined.toOption,
        db->Js.Null_undefined.toOption,
      ) {
      | (Some(e), _) =>
        failwith(
          e
          ->Js.Exn.message
          ->Belt.Option.getWithDefault("unknown_connection_exception"),
        )
      | (None, Some(db)) => callback(db)
      | (None, None) => failwith("Unknown connection error")
      }
    );
  };

  let getFirstRow = result =>
    switch (result->MySql2.Select.rows->Belt.Array.get(0)) {
    | Some(row) => Belt.Result.Ok(row)
    | None => Belt.Result.Error("No rows in result set")
    };

  let decodeRow = row =>
    switch (row->Js.Json.classify) {
    | Js.Json.JSONObject(dict) =>
      switch (dict->Js.Dict.get("result")) {
      | Some(result) =>
        switch (result->Js.Json.decodeNumber) {
        | Some(number) => Belt.Result.Ok(number)
        | None => Belt.Result.Error({j|Result wasn't a number: $result|j})
        }
      | None =>
        Belt.Result.Error({j|Could not find result key in row: $dict|j})
      }
    | _ => Belt.Result.Error({j|Could not decode row object: $row|j})
    };

  describe("connect :: with params :: ", () =>
    testAsync("should connect and allow a query", finish =>
      getDb(connect, db =>
        select(db, "SELECT 1 + 1 AS result", None, response =>
          switch (
            response
            ->Belt.Result.flatMap(getFirstRow)
            ->Belt.Result.flatMap(decodeRow)
            ->Belt.Result.map(x => x |> Expect.expect |> Expect.toEqual(2.0))
          ) {
          | Belt.Result.Ok(assertion) => assertion->finish
          | Belt.Result.Error(message) => message->fail->finish
          }
        )
      )
    )
  );

  describe("connect :: with defaults :: ", () =>
    testAsync("should connect and allow a query", finish =>
      getDb(connectWithDefaults, db =>
        select(db, "SELECT 1 + 1 AS result", None, response =>
          switch (
            response
            ->Belt.Result.flatMap(getFirstRow)
            ->Belt.Result.flatMap(decodeRow)
            ->Belt.Result.map(x => x |> Expect.expect |> Expect.toEqual(2.0))
          ) {
          | Belt.Result.Ok(assertion) => assertion->finish
          | Belt.Result.Error(message) => message->fail->finish
          }
        )
      )
    )
  );
});
