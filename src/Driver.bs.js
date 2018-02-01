'use strict';

var Block  = require("bs-platform/lib/js/block.js");
var Curry  = require("bs-platform/lib/js/curry.js");
var Result = require("./Result.bs.js");

var Query = /* module */[];

function parse(results, fields) {
  var match = Result.parse(results, fields);
  if (match.tag) {
    return /* Select */Block.__(1, [match[0]]);
  } else {
    return /* Mutation */Block.__(0, [match[0]]);
  }
}

function transform(err, results, fields) {
  if (err == null) {
    return parse(results, fields);
  } else {
    return /* Error */Block.__(2, [err]);
  }
}

function handler(cb, err, results, fields) {
  return Curry._1(cb, transform(err, results, fields));
}

function query(conn, sql, cb) {
  conn.query(sql, (function (param, param$1, param$2) {
          return handler(cb, param, param$1, param$2);
        }));
  return /* () */0;
}

function execute(conn, sql, params, cb) {
  if (params[0] >= 381091443) {
    var options = {
      sql: sql,
      values: params[1],
      namedPlaceholders: true
    };
    conn.execute(options, (function (param, param$1, param$2) {
            return handler(cb, param, param$1, param$2);
          }));
    return /* () */0;
  } else {
    var options$1 = {
      sql: sql,
      values: params[1],
      namedPlaceholders: false
    };
    conn.execute(options$1, (function (param, param$1, param$2) {
            return handler(cb, param, param$1, param$2);
          }));
    return /* () */0;
  }
}

exports.Query     = Query;
exports.parse     = parse;
exports.transform = transform;
exports.handler   = handler;
exports.query     = query;
exports.execute   = execute;
/* No side effect */
