
module Config = struct
  type t

  external make :
    ?host:string ->
    ?port:int ->
    ?user:string ->
    ?password:string ->
    ?database:string ->
    unit ->t = "" [@@bs.obj]
end

type t

external createConnection : Config.t -> t = "" [@@bs.module "mysql2" ]

external endConnection : t -> unit = "end" [@@bs.send]

let make ?host ?port ?user ?password ?database _ =
  createConnection
    (Config.make ?host ?port ?user ?password ?database ())

let close conn =
  let _ = endConnection conn in
  ()

module Promise = struct
  let close conn x =
    Js.Promise.resolve(x)
    |> Js.Promise.then_ (fun x -> let _ = close conn in x)
end
