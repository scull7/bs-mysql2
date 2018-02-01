'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var MySql = require("../src/MySql.bs.js");

var conn = Curry._6(MySql.Connection[/* make */1], /* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

Curry._2(MySql.Connection[/* Promise */2][/* close */0], conn, MySql.Promise[/* pipe_with_params */4]("SELECT ? as search", /* array */["%schema"], Promise.resolve(conn)).then((function (value) {
              console.log(value);
              return Promise.resolve(1);
            }))).catch((function () {
        Curry._1(MySql.Connection[/* close */0], conn);
        return Promise.resolve(-1);
      }));

exports.conn = conn;
/* conn Not a pure module */
