'use strict';

var SqlCommon  = require("bs-sql-common/src/SqlCommon.bs.js");
var Connection = require("../src/Connection.bs.js");

var conn = Connection.make(/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

function test_handler(param) {
  switch (param.tag | 0) {
    case 0 : 
        console.log("MUTATION: ", param[0]);
        return /* () */0;
    case 1 : 
        console.log("SELECT: ", param[0]);
        return /* () */0;
    case 2 : 
        console.log("ERROR: ", param[0]);
        return /* () */0;
    
  }
}

SqlCommon.raw(conn, "SHOW DATABASES", test_handler);

var table_sql = "\n  CREATE TABLE IF NOT EXISTS test.simple (\n    id bigint(20) NOT NULL AUTO_INCREMENT\n  , code varchar(32) NOT NULL\n  , PRIMARY KEY(id)\n  )\n";

SqlCommon.raw(conn, table_sql, test_handler);

var simple_insert_sql = "INSERT INTO test.simple (code) VALUES ('foo')";

SqlCommon.raw(conn, simple_insert_sql, test_handler);

var simple_update_sql = "UPDATE test.simple SET code='foo2' WHERE code='foo'";

SqlCommon.raw(conn, simple_update_sql, test_handler);

SqlCommon.raw(conn, "SELECT NULL FROM test.simple WHERE false", test_handler);

SqlCommon.raw(conn, "SELECT * FROM test.simple", test_handler);

Connection.close(conn);

exports.conn              = conn;
exports.test_handler      = test_handler;
exports.table_sql         = table_sql;
exports.simple_insert_sql = simple_insert_sql;
exports.simple_update_sql = simple_update_sql;
/* conn Not a pure module */
