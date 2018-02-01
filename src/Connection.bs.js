'use strict';

var Mysql2 = require("mysql2");

function make(host, port, user, password, database, _) {
  var tmp = { };
  if (host) {
    tmp.host = host[0];
  }
  if (port) {
    tmp.port = port[0];
  }
  if (user) {
    tmp.user = user[0];
  }
  if (password) {
    tmp.password = password[0];
  }
  if (database) {
    tmp.database = database[0];
  }
  return Mysql2.createConnection(tmp);
}

function end_(conn) {
  conn.end();
  return /* () */0;
}

function end_$1(conn, x) {
  return Promise.resolve(x).then((function (x) {
                end_(conn);
                return x;
              }));
}

var Promise$1 = /* module */[/* end_ */end_$1];

exports.make    = make;
exports.end_    = end_;
exports.Promise = Promise$1;
/* mysql2 Not a pure module */
