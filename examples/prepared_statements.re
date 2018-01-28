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
