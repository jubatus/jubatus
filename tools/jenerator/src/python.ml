(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and
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

(* tsushima wrote :*)

open Syntax
open Lib

let comment_out_head = "#"

let make_header conf source filename content =
  make_source conf source filename content comment_out_head
;;
  

(* return : retval = self.client.call(names) *)
let gen_retval func args =
  "retval = self.client.call('" ^ func ^ "', " ^ (String.concat ", " args) ^ ")"
;;

(* return : def func_name (self, args): *)
let gen_def func args =
  "def " ^ func ^ " (self, " ^ (String.concat ", " args) ^ "):"
;;

let rec gen_type t name num = match t with
    | Object -> raise (Unknown_type("Object is not supported"))
    | Bool | Int(_, _) | Float(_) | Raw | String -> 
	if num = "" then name
	else name ^ "[" ^ num ^ "]"
    | Struct s  -> s ^ ".from_msgpack(" ^ name ^ ")"
    | List t -> 
	let name_ = "elim_" ^ name ^ 
	  (if num = "" then "" else "_" ^ num ^ "_") in
	let name_paren = name ^ 
	  (if num = "" then "" else "[" ^ num ^ "]") in
	  "[" ^ (gen_type t name_ "") ^ " for " ^ name_ 
	  ^ " in " ^ name_paren ^ "]"
    | Map(key, value) -> 
	let t1' = (gen_type key ("k_" ^ name) num) in
	let t2' = (gen_type value ("v_" ^ name) num) in
	  "{" ^ t1' ^ " : " ^ t2' ^ " for " ^ 
	    ("k_" ^ name)  ^ "," ^ ("v_" ^ name) ^ 
	    " in " ^ (name ^ ".items()") ^ "}"
    | Tuple [t1; t2] -> 
	"(" ^ gen_type t1 (name ^ "[0]") num ^ "," ^ gen_type t2 (name ^ "[1]") num ^")"
    | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
    | Nullable(t) -> raise (Unknown_type "Nullable is not supported")

let gen_args args = 
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_arg_def f =
  (gen_type f.field_type "retval" "") ^ " " ^ f.field_name
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args ^ ";"
;;

let gen_ret_type = function
  | None -> "" (* TODO (tsushima): OK? or retval? *)
  | Some typ -> gen_type typ "retval" ""
;;

let gen_client_method m =
  let name = m.method_name in
  let args = name
    ::  List.map (fun f -> f.field_name) m.method_arguments in 
  let ret_type = gen_ret_type m.method_return_type in
  let call =
    [(0, gen_def (List.hd args) (List.tl args));
     (1, gen_retval (List.hd args) (List.tl args));
     (1, "return " ^ ret_type)] in
    call

let gen_client s =
  let methods = List.map gen_client_method s.service_methods in
  let constructor = [
    (0, "def __init__ (self, host, port):");
    (1, "address = msgpackrpc.Address(host, port)");
    (1, "self." ^ s.service_name ^ "= msgpackrpc.Client(address)")
  ] in
  let content = concat_blocks (constructor :: methods) in
    List.concat [
      [
	(0, "class " ^ s.service_name ^ ":");
      ];
    indent_lines 1 content
    ]
;;

let gen_message_field f =
  (0, gen_arg_def f ^ ";")
;;

let gen_self_with_comma field_names =
  (List.map (fun s -> (0, "self." ^ s ^ ",")) field_names)

let gen_self_without_comma field_names =
  (List.map (fun s -> (0, "self." ^ s ^ " = " ^ s)) field_names)

let gen_to_msgpack field_names =
  List.concat [
    [(0, "def to_msgpack(self):")];
    [(1, "return (")];
    indent_lines 2 (gen_self_with_comma field_names);
    [(1, "  )")]
  ]

let rec gen_from_msgpack_types field_types =
  let rec loop lst num = match lst with
    | [] -> []
    | [t] -> [(2, gen_type t "arg" (string_of_int num))]
    | t :: rest -> 
	(2, (gen_type t "arg" (string_of_int num)) ^ ",") :: (loop rest (num + 1)) 
  in loop field_types 0
    
let gen_from_msgpack field_names field_types s =
  List.concat [
    [(0, "@staticmethod")];
    [(0, "def from_msgpack(arg):")];
    [(1, "return " ^ s ^ "(")];
    (gen_from_msgpack_types field_types);
    [(1, ")")]
  ]

let gen_message m =
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  List.concat [
    [
      (0, "class " ^ m.message_name ^ ":");
      (1, gen_def "__init__" field_names);
    ];
    indent_lines 2 (gen_self_without_comma field_names);
    [(0, "")];
    indent_lines 1 (gen_to_msgpack field_names);
    [(0, "")];
    indent_lines 1 (gen_from_msgpack field_names field_types m.message_name);
  ]
;;

let gen_typedef = function
  | Typedef(name, typ) ->
      (* TODO : OK ?*)
      []
  | Message m ->
    gen_message m
  | _ ->
    []
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_client.py" in
  let clients = List.map gen_client services in

  let content = concat_blocks [
    [
      (0, "import msgpackrpc");
      (0, "from types import *")
    ];
    (concat_blocks clients)
  ]
  in make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = base ^ "_types.py" in
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "import sys");
    (0, "import msgpack")
  ] in

  let content = concat_blocks [
    includes;
    (concat_blocks types)
  ] in

  make_header conf source name content
;;


let generate conf source idl =
  let services = get_services idl in
    gen_client_file conf source services;
    gen_type_file conf source idl 
;;
