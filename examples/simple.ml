let conn = MySql.Connection.make ~host:"127.0.0.1" ~port:3306 ~user:"root" ()

let test_handler = function
  | Driver.Error e ->
    let _ = Js.log "ERROR: " in Js.log e
  | Driver.Select s ->
    let _ = Js.log "Select: " in Js.log s
  | Driver.Mutation m ->
    let _ = Js.log "Mutation: " in Js.log m


let _ = MySql.raw conn "SHOW DATABASES" test_handler

let table_sql = {|
  CREATE TABLE IF NOT EXISTS test.simple (
    id bigint(20) NOT NULL AUTO_INCREMENT
  , code varchar(32) NOT NULL
  , PRIMARY KEY(id)
  )
|}

let _ = MySql.raw conn table_sql test_handler

let simple_insert_sql = "INSERT INTO test.simple (code) VALUES ('foo')"

let _ = MySql.raw conn simple_insert_sql test_handler

let simple_update_sql = "UPDATE test.simple SET code='foo2' WHERE code='foo'"

let _ = MySql.raw conn simple_update_sql test_handler

let _ = MySql.raw
  conn
  "SELECT NULL FROM test.simple WHERE false"
  test_handler

let _ = MySql.raw conn "SELECT * FROM test.simple" test_handler

let _ = MySql.Connection.close conn
