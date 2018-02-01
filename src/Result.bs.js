'use strict';

var Block        = require("bs-platform/lib/js/block.js");
var Curry        = require("bs-platform/lib/js/curry.js");
var Js_json      = require("bs-platform/lib/js/js_json.js");
var Pervasives   = require("bs-platform/lib/js/pervasives.js");
var Js_primitive = require("bs-platform/lib/js/js_primitive.js");

function andThenClassify(f, param) {
  if (param) {
    return Curry._1(f, Js_json.classify(param[0]));
  } else {
    return /* None */0;
  }
}

function parse_int_default(x, maybe_json) {
  if (maybe_json) {
    var match = Js_json.classify(maybe_json[0]);
    if (typeof match === "number") {
      if (match === 2) {
        return x;
      } else {
        return Pervasives.failwith("parse_int_unexpected_value");
      }
    } else if (match.tag === 1) {
      return match[0] | 0;
    } else {
      return Pervasives.failwith("parse_int_unexpected_value");
    }
  } else {
    return x;
  }
}

function parse_int_maybe(param) {
  return andThenClassify((function (x) {
                if (typeof x === "number") {
                  if (x === 2) {
                    return /* None */0;
                  } else {
                    return Pervasives.failwith("parse_int_maybe_unexecpted_value");
                  }
                } else if (x.tag === 1) {
                  var n = x[0];
                  if (n !== 0.0) {
                    return /* Some */[n | 0];
                  } else {
                    return /* None */0;
                  }
                } else {
                  return Pervasives.failwith("parse_int_maybe_unexecpted_value");
                }
              }), param);
}

function parse_string_maybe(param) {
  return andThenClassify((function (x) {
                if (typeof x === "number") {
                  return Pervasives.failwith("parse_string_maybe_unexpected_value");
                } else if (x.tag) {
                  return Pervasives.failwith("parse_string_maybe_unexpected_value");
                } else {
                  var s = x[0];
                  if (s === "") {
                    return /* None */0;
                  } else {
                    return /* Some */[s];
                  }
                }
              }), param);
}

function parse_mutation(m) {
  return /* record */[
          /* affected_rows */parse_int_default(0, Js_primitive.undefined_to_opt(m["affectedRows"])),
          /* insert_id */parse_int_maybe(Js_primitive.undefined_to_opt(m["insertId"])),
          /* info */parse_string_maybe(Js_primitive.undefined_to_opt(m["info"])),
          /* server_status */parse_int_maybe(Js_primitive.undefined_to_opt(m["serverStatus"])),
          /* warning_status */parse_int_default(0, Js_primitive.undefined_to_opt(m["warningStatus"])),
          /* changed_rows */parse_int_default(0, Js_primitive.undefined_to_opt(m["changedRows"]))
        ];
}

function parse_select(s, fields) {
  return /* record */[
          /* rows */s,
          /* fields */fields
        ];
}

function assert_fields(fields, json) {
  if (fields == null) {
    return Pervasives.failwith("invalid_driver_result_no_column_data");
  } else {
    return /* ResultSelect */Block.__(1, [/* record */[
                /* rows */json,
                /* fields */fields
              ]]);
  }
}

function parse(json, fields) {
  var match = Js_json.classify(json);
  if (typeof match === "number") {
    return Pervasives.failwith("invalid_driver_result");
  } else {
    switch (match.tag | 0) {
      case 2 : 
          return /* ResultMutation */Block.__(0, [parse_mutation(match[0])]);
      case 3 : 
          return assert_fields(fields, match[0]);
      default:
        return Pervasives.failwith("invalid_driver_result");
    }
  }
}

exports.andThenClassify    = andThenClassify;
exports.parse_int_default  = parse_int_default;
exports.parse_int_maybe    = parse_int_maybe;
exports.parse_string_maybe = parse_string_maybe;
exports.parse_mutation     = parse_mutation;
exports.parse_select       = parse_select;
exports.assert_fields      = assert_fields;
exports.parse              = parse;
/* No side effect */
