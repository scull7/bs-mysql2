
type t

external make :
  ?host:string ->
  ?port:int ->
  ?user:string ->
  ?password:string ->
  ?database:string ->
  unit -> t = "" [@@bs.obj]
