(*
let () =
  Jest.describe "Raw SQL Query Test"  (fun () ->
    let db =
      ref (MySql.Connection.make ~host:"127.0.0.1" ~port:3306 ~user:"root" ());
    in
      Jest.afterAll (fun () -> MySql.Connection.end_ !db);
      Jest.testPromise "receive result" (fun () ->
        Js.Promise.resolve !db
        |> MySql.Promise.pquery ~sql: "SELECT ? + ? AS result" ~placeholders: [| 5, 6 |]
        |> Js.Promise.then_ (fun res ->
            let (rows, _) = res in
              match rows with
              | [] -> Js.Promise.reject (Failure "Empty result set")
              | _ -> Js.Promise.resolve Jest.pass
          )
      );
  );
*)
