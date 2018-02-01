'use strict';

var SqlCommon  = require("bs-sql-common/src/SqlCommon.bs.js");
var Connection = require("../src/Connection.bs.js");

var conn = Connection.make(/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

Connection.Promise[/* close */0](conn, SqlCommon.Promise[/* pipe_with_params */4]("SELECT ? as search", /* array */["%schema"], Promise.resolve(conn)).then((function (value) {
              console.log(value);
              return Promise.resolve(1);
            }))).catch((function () {
        Connection.close(conn);
        return Promise.resolve(-1);
      }));

exports.conn = conn;
/* conn Not a pure module */
