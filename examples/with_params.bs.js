'use strict';

var SqlCommon  = require("bs-sql-common/src/SqlCommon.bs.js");
var Connection = require("../src/Connection.bs.js");

var conn = Connection.make(/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

function logThenClose(label, x) {
  console.log(label, x);
  return Connection.close(conn);
}

var sql1 = "SELECT 1 + ? + ? as result";

SqlCommon.with_params(conn, sql1, /* int array */[
      5,
      6
    ], (function (r) {
        switch (r.tag | 0) {
          case 0 : 
              console.log("MUTATION: ", r[0]);
              return /* () */0;
          case 1 : 
              console.log("SELECT: ", r[0]);
              return /* () */0;
          case 2 : 
              console.log("ERROR: ", r[0]);
              return /* () */0;
          
        }
      }));

var sql2 = "SELECT :x + :y as z";

var params2 = {
  x: 1,
  y: 2
};

SqlCommon.with_named_params(conn, sql2, params2, (function (r) {
        switch (r.tag | 0) {
          case 0 : 
              return logThenClose("MUTATION: ", r[0]);
          case 1 : 
              return logThenClose("SELECT: ", r[0]);
          case 2 : 
              return logThenClose("ERROR: ", r[0]);
          
        }
      }));

exports.conn         = conn;
exports.logThenClose = logThenClose;
exports.sql1         = sql1;
exports.sql2         = sql2;
exports.params2      = params2;
/* conn Not a pure module */
