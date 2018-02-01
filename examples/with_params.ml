let conn = MySql.Connection.make ~host:"127.0.0.1" ~port:3306 ~user:"root" ()

let logThenClose label x =
  let _ = Js.log2 label x in
  MySql.Connection.close conn

let sql1 = "SELECT 1 + ? + ? as result"
let _ = MySql.Query.with_params conn sql1 [|5; 6|] (fun r ->
  match r with
  | Response.Error e -> Js.log2 "ERROR: " e
  | Response.Select s -> Js.log2 "SELECT: " s
  | Response.Mutation m -> Js.log2 "MUTATION: " m
)

let sql2 = "SELECT :x + :y as z"
let params2 = [%bs.obj {x = 1; y = 2}]
let _ = MySql.Query.with_named_params conn sql2 params2 (fun r ->
  match r with
  | Response.Error e -> logThenClose "ERROR: " e
  | Response.Select s -> logThenClose "SELECT: " s
  | Response.Mutation m -> logThenClose "MUTATION: " m
)
