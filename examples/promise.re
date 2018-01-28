let conn =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Js.Promise.resolve(conn)
|> Mysql.Promise.pquery
     /* Guess the driver doesn't interpolate for SHOW statements */
     /* ~sql="SHOW DATABASES LIKE ?", */
     (~sql="SELECT ? as search", ~placeholders=[|"%schema"|])
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
