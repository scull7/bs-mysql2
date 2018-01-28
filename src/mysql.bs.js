'use strict';

var Mysql2            = require("mysql2");
var Js_null_undefined = require("bs-platform/lib/js/js_null_undefined.js");

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

function unnamed(connection, sql, placeholders, callback) {
  var params = Js_null_undefined.from_opt(placeholders);
  connection.execute(sql, params, callback);
  return /* () */0;
}

function named(connection, sql, placeholders, callback) {
  var options = {
    sql: sql,
    values: Js_null_undefined.from_opt(placeholders),
    namedPlaceholders: true
  };
  connection.execute(options, callback);
  return /* () */0;
}

var Execute = /* module */[
  /* unnamed */unnamed,
  /* named */named
];

function query(connection, sql, callback) {
  connection.query(sql, callback);
  return /* () */0;
}

function create_response(results, fields) {
  return /* record */[
          /* fields */fields,
          /* results */results
        ];
}

function query$1(connection, string) {
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

var Promise$1 = /* module */[
  /* create_response */create_response,
  /* query */query$1
];

exports.Connection = Connection;
exports.Execute    = Execute;
exports.query      = query;
exports.Promise    = Promise$1;
/* mysql2 Not a pure module */
