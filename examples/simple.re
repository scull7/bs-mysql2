let conn = MySql2.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let test_handler =
  fun
  | `Error(e) => Js.log2("ERROR: ", e)
  | `Select(rows, meta) => Js.log3("SELECT: ", rows, meta)
  | `Mutation(count, id) => Js.log3("MUTATION: ", count, id);

let _ = MySql2.execute(conn, "SHOW DATABASES", None, test_handler);

let table_sql = {|
  CREATE TABLE IF NOT EXISTS test.simple (
    id bigint(20) NOT NULL AUTO_INCREMENT
  , code varchar(32) NOT NULL
  , PRIMARY KEY(id)
  )
|};

let _ = MySql2.execute(conn, table_sql, None, test_handler);

let simple_insert_sql = "INSERT INTO test.simple (code) VALUES ('foo')";

let _ = MySql2.execute(conn, simple_insert_sql, None, test_handler);

let simple_update_sql = "UPDATE test.simple SET code='foo2' WHERE code='foo'";

let _ = MySql2.execute(conn, simple_update_sql, None, test_handler);

let _ =
  MySql2.execute(
    conn,
    "SELECT NULL FROM test.simple WHERE false",
    None,
    test_handler,
  );

let _ = MySql2.execute(conn, "SELECT * FROM test.simple", None, test_handler);

let _ = MySql2.close(conn);
