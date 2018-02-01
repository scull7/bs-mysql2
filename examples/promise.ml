let conn = MySql.Connection.make ~host:"127.0.0.1" ~port:3306 ~user:"root" ()

let _ = Js.Promise.resolve(conn)
|> MySql.Promise.pipe_with_params "SELECT ? as search" [| "%schema" |]
|> Js.Promise.then_ (fun value ->
    let _ = Js.log value in
    Js.Promise.resolve(1)
  )
|> MySql.Connection.Promise.close conn
|> Js.Promise.catch (fun err ->
    let _ = Js.log2("Failure!!!", err) in
    let _ = MySql.Connection.close conn in
    Js.Promise.resolve(-1)
  )
