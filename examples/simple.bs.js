'use strict';

var Mysql = require("../src/mysql.bs.js");

var conn = Mysql.createConnection(/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

conn.query("SHOW DATABASES", (function (error, results, fields) {
        if (error == null) {
          console.log(results);
          console.log(fields);
          return /* () */0;
        } else {
          console.log(error.message);
          return /* () */0;
        }
      }));

conn.end();

exports.conn = conn;
/* conn Not a pure module */
