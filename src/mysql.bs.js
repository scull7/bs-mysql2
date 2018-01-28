'use strict';

var Mysql2 = require("mysql2");

function createConnection(host, port, user, password, database, _) {
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

exports.createConnection = createConnection;
/* mysql2 Not a pure module */
