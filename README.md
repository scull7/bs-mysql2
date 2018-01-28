![Project Status](https://img.shields.io/badge/status-alpha-red.svg)

# bs-mysql2
ReasonML bindings to the [mysql2][mysql2] library.

This is a very rough implementation that will enable very simple use cases.

Initially this was just a copy of [bs-mysql][bs-mysql].

## Usage

### Standard Callback Interface
```reason
let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Mysql.query(conn, "SHOW DATABASES", (error, results, fields) =>
  switch (Js.Nullable.to_opt(error)) {
  | None =>
    Js.log(results);
    Js.log(fields);
  | Some(error) => Js.log(error##message)
  }
);

Mysql.Connection.end_(conn);
```

### Promise Interface
```reason
let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Mysql.Promise.query(conn, "SHOW DATABASES")
|> Js.Promise.then_(value => {
     Js.log(value);
     Js.Promise.resolve(1);
   })
|> Js.Promise.catch((err: Js.Promise.error) => {
     Js.log2("Failure!!", err);
     Js.Promise.resolve(-1);
   });

Mysql.Connection.end_(conn);
```

## How do I install it?

Inside of a BuckleScript project:
```shell
yarn install --save bs-mysql2
```

Then add `bs-mysql2` to your `bs-dependencies` in `bsconfig.json`:
```json
{
  "bs-dependencies": [
    "bs-mysql2"
  ]
}
```

## How do I use it?

### Use it in your project
See the [Usage](#usage) section above...

### Run the examples
```shell
yarn run examples:simple
```

## What's missing?

Mostly everything...

[mysql2]: https://www.npmjs.com/package/mysql2
[bs-mysql]: https://github.com/davidgomes/bs-mysql
