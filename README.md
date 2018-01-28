![Project Status](https://img.shields.io/badge/status-alpha-red.svg)

# bs-mysql2
ReasonML bindings to the [mysql2][mysql2] library.

This is a very rough implementation that will enable very simple use cases.

Initially this was just a copy of [bs-mysql][bs-mysql].

## Usage

### Standard Callback Interface

#### Standard Query Method
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

#### Prepared Statements

##### Named Placeholders
```reason
let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Mysql.Execute.named(
  ~connection=conn,
  ~sql="SELECT :x + :y as z",
  ~placeholders={"x": 1, "y": 2},
  (err, rows, fields) =>
  switch (Js.Nullable.to_opt(err)) {
  | None =>
    Js.log(rows);
    Js.log(fields);
  | Some(err) => Js.log(err)
  }
);

Mysql.Connection.end_(conn);
```

##### Unnamed Placeholders
```reason
let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Mysql.Execute.unnamed(
  ~connection=conn,
  ~sql="SELECT 1 + ? + ? as result",
  ~placeholders=[|5, 6|],
  (err, rows, fields) =>
  switch (Js.Nullable.to_opt(err)) {
  | None =>
    Js.log(rows);
    Js.log(fields);
  | Some(err) => Js.log(err)
  }
);

Mysql.Connection.end_(conn);
```

### Promise Interface
```reason
let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

/* Here we use the pipe query function (pquery) */
Js.Promise.resolve(conn)
|> Mysql.Promise.pquery
    (~sql="SELECT ? as search", ~placeholders=[|"%schema"|])
/* Alternatively we could use the normal query method
|> Js.Promise.then_(Mysql.Promise.query(
    ~sql="SELECT ? as search",
    ~placeholders=[|"%something"|]
  ))
*/
|> Js.Promise.then_(value => {
     Js.log(value);
     Js.Promise.resolve(1);
   })
|> Mysql.Promise.Connection.end_(conn)
|> Js.Promise.catch((err: Js.Promise.error) => {
     Js.log2("Failure!!", err);
     Mysql.Connection.end_(conn);
     Js.Promise.resolve(-1);
   });

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
