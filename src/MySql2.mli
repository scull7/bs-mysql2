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

  val make :
    ?host:string ->
    ?port:int ->
    ?user:string ->
    ?password:string ->
    ?database:string ->
    unit -> t

  val close : t -> unit
end

module Error : sig
  type t

  val name : t -> string
  val message : t -> string
  val code : t -> string
  val errno : t -> int
  val sql_state : t -> string option
  val sql_message : t -> string option
end

type connection = Connection.t
type callback =
  [ `Error of Error.t
  | `Mutation of int * int
  | `Select of rows * meta
  ] ->
  unit

val close : Connection.t -> unit

val execute : Connection.t -> string -> params -> callback -> unit

val parse_response :
  Js.Json.t ->
  Js.Json.t array ->
  [> `Error of Error.t
  |  `Mutation of int * int
  |  `Select of rows * meta
  ]
