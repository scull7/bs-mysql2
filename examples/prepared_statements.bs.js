'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var MySql = require("../src/MySql.bs.js");

var conn = Curry._6(MySql.Connection[/* make */1], /* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

MySql.with_params(conn, "SELECT 1 + ? + ? as result", /* int array */[
      5,
      6
    ], (function (result) {
        switch (result.tag | 0) {
          case 0 : 
              console.log("MUTATION: ", result[0]);
              return /* () */0;
          case 1 : 
              console.log("SELECT: ", result[0]);
              return /* () */0;
          case 2 : 
              console.log("ERROR: ", result[0]);
              return /* () */0;
          
        }
      }));

MySql.with_named_params(conn, "SELECT :x + :y as z", {
      x: 1,
      y: 2
    }, (function (result) {
        switch (result.tag | 0) {
          case 0 : 
              console.log("MUTATION: ", result[0]);
              return /* () */0;
          case 1 : 
              console.log("SELECT: ", result[0]);
              return /* () */0;
          case 2 : 
              console.log("ERROR: ", result[0]);
              return /* () */0;
          
        }
      }));

Curry._1(MySql.Connection[/* close */0], conn);

exports.conn = conn;
/* conn Not a pure module */
