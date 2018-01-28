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

function end_(connection) {
  connection.end();
  return /* () */0;
}

var Connection = /* module */[
  /* make */make,
  /* end_ */end_
];

function create_response(results, fields) {
  return /* record */[
          /* fields */fields,
          /* results */results
        ];
}

function query(connection, string) {
  return new Promise((function (resolve, reject) {
                connection.query(string, (function (error, results, fields) {
                        if (error == null) {
                          return resolve(/* record */[
                                      /* fields */fields,
                                      /* results */results
                                    ]);
                        } else {
                          return reject(error);
                        }
                      }));
                return /* () */0;
              }));
}

var Promise$1 = /* module */[/* query */query];

exports.Connection      = Connection;
exports.create_response = create_response;
exports.Promise         = Promise$1;
/* mysql2 Not a pure module */
