(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.

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

type decl_type = Void | Object | Bool
		 | Byte | Short | Int | Long 
		 | Ubyte | Ushort | Uint | Ulong
		 | Float | Double | Raw | String 
		 | Struct of string
		 | List of decl_type
		 | Map of decl_type * decl_type
		 | Tuple of decl_type list
		 | Nullable of decl_type 
(*		 | `Null *)

type field_type = Field of int * decl_type * string * string list

type routing_type = Random | Cht of int | Broadcast | Internal
type reqtype = Update | Analysis | Nolock

(* known_aggregators =
   ["#@all_and"; "#@all_or"; "#@concat"; "#@merge"; "#@ignore";"#@pass"] in  *)
type aggtype = All_and | All_or | Concat | Merge | Ignore | Pass

type decorator_type = Routing of routing_type
		      | Reqtype of reqtype
		      | Aggtype of aggtype

type method_type = Method of decl_type * string * ( field_type list) * (decorator_type list) * string list

type stree = Typedef of string * decl_type * string list
	     | Enum of string * (int * string) list * string list
	     | Message of string * field_type list * string list
	     | Exception of string * field_type list * string * string list
	     | Service of string * method_type list * string list

let known_types = Hashtbl.create 256;;

let _ =
  List.iter (fun s -> Hashtbl.add known_types s 0) [             
    "int"; (*"void"; "char"; "double" *)
  ];;

exception Unknown_type of string

let add_known_types name =
  Hashtbl.add known_types name 0;;

let check_type name =
  Hashtbl.mem known_types name;;

let make_decorator = function
  | "#@update"   -> Reqtype(Update);
  | "#@analysis" -> Reqtype(Analysis);
  | "#@nolock"   -> Reqtype(Nolock);

  | "#@random"   -> Routing(Random);
  | "#@broadcast" -> Routing(Broadcast);
  | "#@internal"  -> Routing(Internal);
  | "#@cht"       -> Routing(Cht(2));

  | "#@all_and"   -> Aggtype(All_and);
  | "#@all_or"   -> Aggtype(All_or);
  | "#@concat"    -> Aggtype(Concat);
  | "#@merge"    -> Aggtype(Merge);
  | "#@ignore"    -> Aggtype(Ignore);
  | "#@pass"      -> Aggtype(Pass);
  | other -> raise (Unknown_type other);;

let make_decorator_with_int d i =
  match d with
    | "#@cht" when 0 <= i -> Routing(Cht(i))
    | "#@cht" -> raise (Unknown_type "cht with negative i");
    | other -> raise (Unknown_type other);;

let routing_to_string = function
  | Random -> "random";
  | Cht(i) -> "cht("^(string_of_int i)^")";
  | Broadcast -> "broadcast";
  | Internal -> "";;

let aggtype_to_string = function
  | All_and -> "all_and";
  | All_or  -> "all_or";
  | Concat  -> "concat";
  | Merge   -> "merge";
  | Ignore  -> "ignore"; (* or raise sth? *)
  | Pass -> "pass";;

let reqtype_to_string = function
  | Update -> "update";
  | Analysis -> "analysis";
  | Nolock -> "nolock";;

let decorator_to_string = function
  | Reqtype(r) -> "Reqtype("^(reqtype_to_string r)^")";
  | Routing(r) -> "Routing("^(routing_to_string r)^")";
  | Aggtype(r) -> "Aggtype("^(aggtype_to_string r)^")";;
