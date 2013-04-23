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

(* tsushima wrote *)

open Syntax
open Lib

let comment_out_head = "#"
;;

let make_header conf source filename content =
  make_source conf source filename content comment_out_head
;;

(* return : retval = self.client.call(names) *)
let gen_retval = function
  | [] -> assert false
      (* Because m.method_name (@gen_client_method) can not be empty *)
  | func :: [] ->
      "retval = self.client.call('" ^ func ^ "')"
  | func :: args ->
      "retval = self.client.call('" ^ func ^ "', " ^ (String.concat ", " args) ^ ")"
;;

(* return : def func_name (self, args): *)
let gen_def = function
  | [] -> assert false
  | func :: [] ->
      "def " ^ func ^ " (self):"
  | func :: args ->
      "def " ^ func ^ " (self, " ^ (String.concat ", " args) ^ "):"
;;

let gen_typename_with_paren name num = 
  if num = "" then name
  else name ^ "[" ^ num ^ "]"
;;

let gen_typename_with_bar name num = 
  if num = "" then name
  else name ^ "_" ^ num ^ "_"
;;

let rec gen_type t name num = match t with
  | Object -> raise (Unknown_type("Object is not supported"))
  | Bool | Int(_, _) | Float(_) | Raw | String -> 
    gen_typename_with_paren name num
  | Struct s  -> s ^ ".from_msgpack(" ^ gen_typename_with_paren name num ^ ")"
  | List t -> 
    let name_bar = "elem_" ^ (gen_typename_with_bar name num) in
    let name_paren = gen_typename_with_paren name num in
    "[" ^ (gen_type t name_bar "") ^ " for " ^ name_bar 
    ^ " in " ^ name_paren ^ "]"
  | Map(key, value) -> 
    let new_name_bar = (gen_typename_with_bar name num) in
    let new_name_paren = (gen_typename_with_paren name num) in
    let t1' = (gen_type key ("k_" ^ new_name_bar) "") in
    let t2' = (gen_type value ("v_" ^ new_name_bar) "") in
    "{" ^ t1' ^ " : " ^ t2' ^ " for " ^ 
      ("k_" ^ new_name_bar)  ^ "," ^ ("v_" ^ new_name_bar) ^ 
      " in " ^ new_name_paren ^ ".items()" ^ "}"
  | Tuple [t1; t2] -> 
    " (" ^ gen_type t1 (name ^ "[0]") num ^ ", " ^ gen_type t2 (name ^ "[1]") num ^") "
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
;;

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
  | None -> "retval" (* TODO (tsushima): OK? *)
  | Some typ -> gen_type typ "retval" ""
;;

let gen_client_method m =
  let name = m.method_name in
  let args = name
    ::  List.map (fun f -> f.field_name) m.method_arguments in 
  let ret_type = gen_ret_type m.method_return_type in
  let call =
    [(0, gen_def args);
     (1, gen_retval args);
     (1, "return " ^ ret_type)] in
    call
;;

let gen_client s =
  let methods = List.map gen_client_method s.service_methods in
  let constructor = [
    (0, "def __init__ (self, host, port):");
    (1, "address = msgpackrpc.Address(host, port)");
    (1, "self.client = msgpackrpc.Client(address)");
    (0, "");
    (0, "def get_client (self):");
    (1, "return self.client")
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
;;

let gen_self_without_comma field_names =
  (List.map (fun s -> (0, "self." ^ s ^ " = " ^ s)) field_names)
;;

let gen_to_msgpack field_names =
  List.concat [
    [(0, "def to_msgpack (self):")];
    [(1, "return (")];
    indent_lines 2 (gen_self_with_comma field_names);
    [(1, "  )")]
  ]
;;

let mapi f lst =
  let rec mapi_impl f i = function
    | [] -> []
    | x::xs -> f i x :: mapi_impl f (i + 1) xs in
  mapi_impl f 0 lst
;;

let rec gen_from_msgpack_types field_types =
  mapi (fun i t -> gen_type t "arg" (string_of_int i)) field_types
;;

let gen_from_msgpack field_names field_types s =
  let args = gen_from_msgpack_types field_types in
  [
    (0, "@staticmethod");
    (0, "def from_msgpack (arg):");
    (1,   "return " ^ s ^ "(" ^ String.concat ", " args ^ ")");
  ]
;;

let gen_message m =
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  List.concat [
    [
      (0, "class " ^ m.message_name ^ ":");
      (1, gen_def ("__init__"::field_names));
    ];
    indent_lines 2 (gen_self_without_comma field_names);
    [(0, "")];
    indent_lines 1 (gen_to_msgpack field_names);
    [(0, "")];
    indent_lines 1 (gen_from_msgpack field_names field_types m.message_name);
  ]
;;

let gen_from_msgpack_for_typedef typ =
  let arg = gen_type typ "arg" "" in
  [
    (0, "@staticmethod");
    (0, "def from_msgpack (arg):");
    (1,   "return " ^ arg);
  ]
;;

let gen_typedef' name typ = 
  List.concat [
    [
      (0, "class " ^ name ^ ":");
    ];
    indent_lines 1 (gen_from_msgpack_for_typedef typ);
  ]
;;


let gen_typedef = function
  | Typedef(name, typ) ->
      gen_typedef' name typ
  | Message m ->
    gen_message m
  | _ ->
    []
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = Filename.concat base "client.py" in
  let clients = List.map gen_client services in
  let content = concat_blocks [
    [
      (0, "");
      (0, "import msgpackrpc");
      (0, "from types import *");
    ];
    (concat_blocks clients)
  ]
  in make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = Filename.concat base "types.py" in
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "");
    (0, "import sys");
    (0, "import msgpack");
  ] in
  let content = concat_blocks [
    includes;
    (concat_blocks types)
  ] in
  make_header conf source name content
;;

let gen_init_file conf source =
  let base = File_util.take_base source in
  let name = Filename.concat base "__init__.py" in
  make_header conf source name []
;;

let generate conf source idl =
  let services = get_services idl in
  gen_client_file conf source services;
  gen_type_file conf source idl;
  gen_init_file conf source
;;
