module Query = SqlCommon
module Promise = Query.Promise
module Response = SqlCommon.Response

module Connection = struct
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

  external close : t -> unit = "end" [@@bs.send]

  let make ?host ?port ?user ?password ?database _ =
    createConnection
      (Config.make ?host ?port ?user ?password ?database ())

  module Promise = struct
    let close conn x =
      Js.Promise.resolve(x)
      |> Js.Promise.then_ (fun x -> let _ = close conn in x)
  end
end
