let conn = MySql.Connection.make ~host:"127.0.0.1" ~port:3306 ~user:"root" ()

let test_handler = function
  | Response.Error e -> Js.log2 "ERROR: " e
  | Response.Select s -> Js.log2 "SELECT: " s
  | Response.Mutation m -> Js.log2 "MUTATION: " m

let _ = MySql.Query.raw conn "SHOW DATABASES" test_handler

let table_sql = {|
  CREATE TABLE IF NOT EXISTS test.simple (
    id bigint(20) NOT NULL AUTO_INCREMENT
  , code varchar(32) NOT NULL
  , PRIMARY KEY(id)
  )
|}

let _ = MySql.Query.raw conn table_sql test_handler

let simple_insert_sql = "INSERT INTO test.simple (code) VALUES ('foo')"

let _ = MySql.Query.raw conn simple_insert_sql test_handler

let simple_update_sql = "UPDATE test.simple SET code='foo2' WHERE code='foo'"

let _ = MySql.Query.raw conn simple_update_sql test_handler

let _ = MySql.Query.raw
  conn
  "SELECT NULL FROM test.simple WHERE false"
  test_handler

let _ = MySql.Query.raw conn "SELECT * FROM test.simple" test_handler

let _ = MySql.Connection.close conn
