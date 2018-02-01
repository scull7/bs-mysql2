'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var MySql = require("../src/MySql.bs.js");

var conn = Curry._6(MySql.Connection[/* make */1], /* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

function test_handler(param) {
  switch (param.tag | 0) {
    case 0 : 
        console.log("Mutation: ");
        console.log(param[0]);
        return /* () */0;
    case 1 : 
        console.log("Select: ");
        console.log(param[0]);
        return /* () */0;
    case 2 : 
        console.log("ERROR: ");
        console.log(param[0]);
        return /* () */0;
    
  }
}

MySql.raw(conn, "SHOW DATABASES", test_handler);

var table_sql = "\n  CREATE TABLE IF NOT EXISTS test.simple (\n    id bigint(20) NOT NULL AUTO_INCREMENT\n  , code varchar(32) NOT NULL\n  , PRIMARY KEY(id)\n  )\n";

MySql.raw(conn, table_sql, test_handler);

var simple_insert_sql = "INSERT INTO test.simple (code) VALUES ('foo')";

MySql.raw(conn, simple_insert_sql, test_handler);

var simple_update_sql = "UPDATE test.simple SET code='foo2' WHERE code='foo'";

MySql.raw(conn, simple_update_sql, test_handler);

MySql.raw(conn, "SELECT NULL FROM test.simple WHERE false", test_handler);

MySql.raw(conn, "SELECT * FROM test.simple", test_handler);

Curry._1(MySql.Connection[/* close */0], conn);

exports.conn              = conn;
exports.test_handler      = test_handler;
exports.table_sql         = table_sql;
exports.simple_insert_sql = simple_insert_sql;
exports.simple_update_sql = simple_update_sql;
/* conn Not a pure module */
