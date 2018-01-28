'use strict';

var Mysql = require("../src/mysql.bs.js");

var conn = Mysql.Connection[/* make */0](/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

Mysql.Execute[/* unnamed */0](conn, "SELECT 1 + ? + ? as result", /* Some */[/* int array */[
        5,
        6
      ]], (function (err, rows, fields) {
        if (err == null) {
          console.log(rows);
          console.log(fields);
          return /* () */0;
        } else {
          console.log(err);
          return /* () */0;
        }
      }));

Mysql.Execute[/* named */1](conn, "SELECT :x + :y as z", /* Some */[{
        x: 1,
        y: 2
      }], (function (err, rows, fields) {
        if (err == null) {
          console.log(rows);
          console.log(fields);
          return /* () */0;
        } else {
          console.log(err);
          return /* () */0;
        }
      }));

Mysql.Connection[/* end_ */1](conn);

exports.conn = conn;
/* conn Not a pure module */
