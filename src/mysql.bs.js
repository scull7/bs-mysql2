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

function end_$1(connection, x) {
  return Promise.resolve(x).then((function (x) {
                connection.end();
                return x;
              }));
}

var Connection$1 = /* module */[/* end_ */end_$1];

function handler(resolve, reject, error, rows, fields) {
  if (error == null) {
    return resolve(/* tuple */[
                rows,
                fields
              ]);
  } else {
    return reject(error);
  }
}

function query$1(connection, sql, placeholders, _) {
  return new Promise((function (resolve, reject) {
                return unnamed(connection, sql, placeholders, (function (param, param$1, param$2) {
                              return handler(resolve, reject, param, param$1, param$2);
                            }));
              }));
}

function pquery(sql, placeholders, pconn) {
  return pconn.then((function (conn) {
                return query$1(conn, sql, placeholders, /* () */0);
              }));
}

var Promise$1 = /* module */[
  /* Connection */Connection$1,
  /* handler */handler,
  /* query */query$1,
  /* pquery */pquery
];

exports.Connection = Connection;
exports.Execute    = Execute;
exports.query      = query;
exports.Promise    = Promise$1;
/* mysql2 Not a pure module */
