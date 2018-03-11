type params =
  [ `Named of Js.Json.t
  | `Positional of Js.Json.t
  ] option
type rows = Js.Json.t array
type meta_record = {
  catalog: string;
  schema: string;
  name: string;
  orgName: string;
  table: string;
  orgTable: string;
  characterSet: int;
  columnLength: int;
  columnType: int;
  flags: int;
  decimals: int
}
type meta = meta_record array

module Connection : sig
  type t
end

module Error : sig
  val from_js : Js.Exn.t -> exn
end

type connection = Connection.t
type callback =
  [ `Error of exn
  | `Mutation of int * int
  | `Select of rows * meta
  ] ->
  unit

val close : Connection.t -> unit

val connect :
    ?host:string ->
    ?port:int ->
    ?user:string ->
    ?password:string ->
    ?database:string ->
    unit -> connection

val execute : Connection.t -> string -> params -> callback -> unit

val parse_response :
  Js.Json.t ->
  Js.Json.t array ->
  [> `Error of exn
  |  `Mutation of int * int
  |  `Select of rows * meta
  ]
