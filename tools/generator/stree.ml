(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

type user_defined_type = Class of string
			 | Struct of string
			 | Template  of string * (anytype list)
			 | Enum of string
(*			 | Namespace of string * user_defined_type  currently no namespace is implemented *)

and  anytype = Void | Int | Char | Double
	       | Other of user_defined_type * bool
	       | Constructor
	       | Destructor

type decorator = string
type prototype = anytype * string * (anytype list) * (decorator list) * bool (* is_const *)
type member    = anytype * string (* static is not allowed, const is not implemented *)

(* currently all members should be public, and 'public:' and 'private:' notations are ignored *)
type class_def = ClassDef of string * (prototype list) * (member list)
type struct_def = StructDef of string * (member list)
type type_def = TypeDef of anytype * string
type enum_def = EnumDef of string * string list

exception Unkown_directive

let known_types = Hashtbl.create 256;;

let _ =
  List.iter (fun (k,v) -> Hashtbl.replace known_types k v) [
    "int", Int;
    "void", Void;
    "char", Char;
    "double", Double;
  ];;

let symbols = Hashtbl.create 256

let _ =
  List.iter (fun (k,v) -> Hashtbl.replace symbols k v) [
    "", "";
  ];;  

let add_known_types name =
  Hashtbl.replace known_types name (Other(Class(name), false));;

let make_template t argv is_ref =
  let newtype = Other( Template(t, argv), is_ref ) in
  Hashtbl.replace known_types t newtype;
  newtype;;

let make_anytype t is_ref =
  try
    Hashtbl.find known_types t
  with
      Not_found ->
	let newtype = Other(Class(t), is_ref) in
	Hashtbl.replace known_types t newtype;
	newtype;;

let find_anytype t =
  Hashtbl.find known_types t

let make_symbol s =
  Hashtbl.replace symbols s s;
  s;;

let rec to_string = function
  | Void -> "void";
  | Int  -> "int";
  | Char -> "char";
  | Double -> "double";
  | Other(ud, is_ref) ->
    let rec ud_type2string = function
      | Class(s) -> s;
      | Struct(s) -> s;
      | Template(n, l) when (n = "tuple" or n = "std::tuple") && ((List.length l) = 2) ->
	let list = List.map to_string l in
	"std::pair<" ^ (String.concat ", " list) ^ "> " ;
      | Template(n, l) ->
	let list = List.map to_string l in
	n ^ "<" ^ (String.concat ", " list) ^ "> " ;
      | Enum(e) ->
	e;
(*      | Namespace(ns, ud) -> ns ^ "::" ^ (ud_type2string ud) *)
    in
    let ref_statement = if is_ref then "&" else "" in
    (ud_type2string ud)^ref_statement;
  | Destructor -> "";
  | Constructor -> "";;

let print_known_types()=
  print_string "\nknown types: ";
  let p_ k _ = print_string (k ^ ", " ) in Hashtbl.iter p_ known_types;
  print_endline "";;

let prototype2string (retval,funcname,argvs,decorators,is_const) =
  let argvs_str =
    String.concat ", " (List.map (fun t-> to_string t) argvs) in
  let const_statement = if is_const then "const" else "" in
  let decorators = String.concat " " decorators in
  Printf.sprintf "=> %s %s(%s)%s;  %s" (* \033[34m escape sequence... for colored term*)
    (to_string retval) funcname argvs_str const_statement decorators;;
  
let print_classdef = function
  | ClassDef(name,funcs,members) ->
    print_endline ("classname => " ^ name ^ ":");
    print_endline " methods:";
    List.iter (Util.compose print_endline prototype2string) funcs;
    let print_member (t,n) =
      Printf.printf "  => %s %s;\n" (to_string t) n
    in
    print_endline " members:";
    List.iter print_member members;;
    
let print_structdef = function
  | StructDef(name, members) ->
    print_endline ("structname => " ^ name ^ ":");
    let print_member (t,n) =
      Printf.printf "  => %s %s;\n" (to_string t) n
    in
    print_endline " members:";
    List.iter print_member members;;

(* prototype -> bool *)
let include_cht_api (_,_,_,decorators,_) = List.mem "//@cht" decorators;;
