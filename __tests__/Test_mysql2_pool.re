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

  describe("connect :: with params :: ", () =>
    testAsync("should connect and allow a query", finish =>
      getDb(connect, db =>
        select(db, "SELECT 1 + 1 AS result", None, response =>
          switch (response) {
          | Belt.Result.Error(msg) => msg->fail->finish
          | Belt.Result.Ok(result) =>
            result
            ->MySql2.Select.rows
            ->Belt.Array.get(0)
            ->Belt.Option.flatMap(Js.Json.decodeObject)
            ->Belt.Option.flatMap(dict => Js.Dict.get(dict, "result"))
            ->Belt.Option.flatMap(Js.Json.decodeNumber)
            ->Belt.Option.map(float =>
                float |> Expect.expect |> Expect.toEqual(2.0)
              )
            ->Belt.Option.getExn
            ->finish
          }
        )
      )
    )
  );

  describe("connect :: with defaults :: ", () =>
    testAsync("should connect and allow a query", finish =>
      getDb(connectWithDefaults, db =>
        select(db, "SELECT 1 + 1 AS result", None, response =>
          switch (response) {
          | Belt.Result.Error(msg) => msg->fail->finish
          | Belt.Result.Ok(result) =>
            result
            ->MySql2.Select.rows
            ->Belt.Array.get(0)
            ->Belt.Option.flatMap(Js.Json.decodeObject)
            ->Belt.Option.flatMap(dict => Js.Dict.get(dict, "result"))
            ->Belt.Option.flatMap(Js.Json.decodeNumber)
            ->Belt.Option.map(float =>
                float |> Expect.expect |> Expect.toEqual(2.0)
              )
            ->Belt.Option.getExn
            ->finish
          }
        )
      )
    )
  );
});
