'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var MySql = require("../src/MySql.bs.js");

var conn = Curry._6(MySql.Connection[/* make */1], /* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

function logThenClose(label, x) {
  console.log(label, x);
  return Curry._1(MySql.Connection[/* close */0], conn);
}

MySql.with_params(conn, "SELECT 1 + ? + ? as result", /* int array */[
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

MySql.with_named_params(conn, sql2, params2, (function (r) {
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
exports.sql2         = sql2;
exports.params2      = params2;
/* conn Not a pure module */
