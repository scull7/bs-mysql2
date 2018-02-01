
type t

external createConnection : Config.t -> t = "" [@@bs.module "mysql2" ]

external endConnection : t -> unit = "end" [@@bs.send]

let make ?host ?port ?user ?password ?database _ =
  createConnection
    (Config.make ?host ?port ?user ?password ?database ())

let end_ conn =
  let _ = endConnection conn in
  ()

module Promise = struct
  let end_ conn x =
    Js.Promise.resolve(x)
    |> Js.Promise.then_ (fun x -> let _ = end_ conn in x)
end
