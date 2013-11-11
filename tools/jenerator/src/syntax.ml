(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and
 Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License version 2.1 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

type decl_type =
  | Object
  | Bool
  | Int of bool * int
  | Float of bool
  | Raw
  | String
  | Datum
  | Struct of string
  | List of decl_type
  | Map of decl_type * decl_type
  | Nullable of decl_type
;;

type field_type = {
  field_number: int;
  field_type: decl_type;
  field_name: string;
};;

type routing_type = | Random | Cht of int | Broadcast | Internal;;

type reqtype = | Update | Analysis | Nolock;;

type aggtype = | All_and | All_or | Concat | Merge | Ignore | Pass;;

type decorator_type =
  | Routing of routing_type
  | Reqtype of reqtype
  | Aggtype of aggtype
;;

type method_type = {
  method_return_type: decl_type option;
  method_name: string;
  method_arguments: field_type list;
  method_decorators: decorator_type list;
};;

let get_decorator m =
  match m.method_decorators with
  | [Routing(rt); Reqtype(rq); Aggtype(ag) ] ->
    (rt, rq, ag)
  | _ ->
    raise (Failure "bad decorator")
;;

type service_type = {
  service_name: string;
  service_methods: method_type list;
};;

type enum_type = {
  enum_name: string;
  enum_values: (int * string) list;
};;

type message_type = {
  message_name: string;
  message_fields: field_type list;
  message_raw: string option;
};;

type exception_type = {
  exception_name: string;
  exception_fields: field_type list;
  exception_super: string option;
};;

type statement =
  | Include of string
  | Enum of enum_type
  | Message of message_type
  | Exception of exception_type
  | Service of service_type
;;

type idl = statement list;;

type program = {
  enums: enum_type list;
  messages: message_type list;
  exceptions: exception_type list;
  services: service_type list;
};;

exception Unknown_type of string;;

let make_decorator = function
  | "#@update"    -> Reqtype(Update)
  | "#@analysis"  -> Reqtype(Analysis)
  | "#@nolock"    -> Reqtype(Nolock)

  | "#@random"    -> Routing(Random)
  | "#@broadcast" -> Routing(Broadcast)
  | "#@internal"  -> Routing(Internal)
  | "#@cht"       -> Routing(Cht(2))

  | "#@all_and"   -> Aggtype(All_and)
  | "#@all_or"    -> Aggtype(All_or)
  | "#@concat"    -> Aggtype(Concat)
  | "#@merge"     -> Aggtype(Merge)
  | "#@ignore"    -> Aggtype(Ignore)
  | "#@pass"      -> Aggtype(Pass)
  | other ->
    raise (Unknown_type other)
;;

let make_decorator_with_int d i =
  match d with
  | "#@cht" when 0 <= i -> Routing(Cht(i))
  | "#@cht" -> raise (Unknown_type "cht with negative i")
  | other -> raise (Unknown_type other)
;;

let routing_to_string = function
  | Random -> "random";
  | Cht(i) -> "cht(" ^ string_of_int i ^ ")";
  | Broadcast -> "broadcast";
  | Internal -> ""
;;

let aggtype_to_string = function
  | All_and -> "all_and"
  | All_or  -> "all_or"
  | Concat  -> "concat"
  | Merge   -> "merge"
  | Ignore  -> "ignore" (* or raise sth? *)
  | Pass    -> "pass"
;;

let reqtype_to_string = function
  | Update   -> "update"
  | Analysis -> "analysis"
  | Nolock   -> "nolock"
;;

let decorator_to_string = function
  | Reqtype(r) -> "Reqtype(" ^ reqtype_to_string r ^ ")"
  | Routing(r) -> "Routing(" ^ routing_to_string r ^ ")"
  | Aggtype(r) -> "Aggtype(" ^ aggtype_to_string r ^ ")"
;;

let rec type_exists f typ =
  f typ ||
  match typ with
  | List t -> type_exists f t
  | Map(k, v) -> type_exists f k || type_exists f v
  | Nullable t -> type_exists f t
  | _ -> false
;;
