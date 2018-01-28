'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Mysql = require("../src/mysql.bs.js");

var conn = Mysql.Connection[/* make */0](/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

Curry._2(Mysql.Promise[/* Connection */0][/* end_ */0], conn, Mysql.Promise[/* pquery */3]("SELECT ? as search", /* Some */[/* array */["%schema"]], Promise.resolve(conn)).then((function (value) {
              console.log(value);
              return Promise.resolve(1);
            }))).catch((function (err) {
        console.log("Failure!!", err);
        Mysql.Connection[/* end_ */1](conn);
        return Promise.resolve(-1);
      }));

exports.conn = conn;
/* conn Not a pure module */
