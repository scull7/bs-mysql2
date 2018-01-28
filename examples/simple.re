let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Mysql.query(conn, "SHOW DATABASES", (error, results, fields) =>
  switch (Js.Nullable.to_opt(error)) {
  | None =>
    Js.log(results);
    Js.log(fields);
  | Some(error) => Js.log(error)
  }
);

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
