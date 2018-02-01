'use strict';

var Driver     = require("./Driver.bs.js");
var Connection = require("./Connection.bs.js");

function with_params(conn, sql, params, cb) {
  return Driver.execute(conn, sql, /* `ParamsUnnamed */[
              -674457606,
              params
            ], cb);
}

function with_named_params(conn, sql, params, cb) {
  return Driver.execute(conn, sql, /* `ParamsNamed */[
              381091443,
              params
            ], cb);
}

var close = Connection.Promise[/* end_ */0];

var Promise = /* module */[/* close */close];

var Connection$1 = /* module */[
  /* close */Connection.end_,
  /* make */Connection.make,
  /* Promise */Promise
];

function handler(resolve, reject, response) {
  switch (response.tag | 0) {
    case 0 : 
    case 1 : 
        return resolve(response);
    case 2 : 
        return reject(response[0]);
    
  }
}

function raw(conn, sql) {
  return new Promise((function (resolve, reject) {
                return Driver.query(conn, sql, (function (param) {
                              return handler(resolve, reject, param);
                            }));
              }));
}

function with_params$1(conn, sql, params) {
  return new Promise((function (resolve, reject) {
                return with_params(conn, sql, params, (function (param) {
                              return handler(resolve, reject, param);
                            }));
              }));
}

function with_named_params$1(conn, sql, params) {
  return new Promise((function (resolve, reject) {
                return with_named_params(conn, sql, params, (function (param) {
                              return handler(resolve, reject, param);
                            }));
              }));
}

function pipe_with_params(sql, params, pconn) {
  return pconn.then((function (conn) {
                return with_params$1(conn, sql, params);
              }));
}

function pipe_with_named_params(sql, params, pconn) {
  return pconn.then((function (conn) {
                return with_named_params$1(conn, sql, params);
              }));
}

var Promise$2 = /* module */[
  /* handler */handler,
  /* raw */raw,
  /* with_params */with_params$1,
  /* with_named_params */with_named_params$1,
  /* pipe_with_params */pipe_with_params,
  /* pipe_with_named_params */pipe_with_named_params
];

var raw$1 = Driver.query;

exports.raw               = raw$1;
exports.with_params       = with_params;
exports.with_named_params = with_named_params;
exports.Connection        = Connection$1;
exports.Promise           = Promise$2;
/* Connection Not a pure module */
