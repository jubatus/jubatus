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

(* same with cpp.ml *)
let indent_line n (ind, line) =
  (ind + n, line)
;;

(* same with cpp.ml *)
let indent_lines n =
  List.map (indent_line n)
;;

(* same with cpp.ml *)
let rfind line pos ch =
  if String.rcontains_from line pos ch then
    Some (String.rindex_from line pos ch)
  else
    None

(* same with cpp.ml *)
let rindex_split_pos line pos =
  let paren = rfind line pos '(' in
  let comma = rfind line pos ',' in
  match paren, comma with
  | Some p, Some c -> Some (max (p + 1) (c + 1))
  | Some p, None -> Some (p + 1)
  | None, Some c -> Some (c + 1)
  | None, None -> None
;;

(* same with cpp.ml *)
let rec print_indent p (indent, line) =
  if line == "" then
    (* don't append spaces to a blank line *)
    p ""
  else
    let space = String.make (indent * 2) ' ' in
    let max_len = 80 - indent * 2 in
    let len = String.length line in
    if len > max_len then
      match rindex_split_pos line (max_len - 1) with
      | Some pos ->
        p (space ^ String.sub line 0 pos);
        print_indent p (indent, "    " ^ String.sub line pos (len - pos))
      | None ->
        p (space ^ line)
    else
      p (space ^ line)
;;

(* same with cpp.ml *)
let print_lines p lines =
  let blank = ref false in
  List.iter (fun (indent, line) ->
    if not (line = "" && !blank) then
      print_indent p (indent, line);
    blank := line = ""
  ) lines
;;

(* same with cpp.ml *)
let rec concat_blocks blocks =
  let rec insert_blank = function
    | [] -> []
    | x::[] -> [x]
    | x::xs -> x::[(0, "")]::(insert_blank xs) in
  List.concat (insert_blank blocks)
;;

(* same with cpp.ml *)
let make_header_message source =
  let file = Filename.basename source in
  [
    "## This file is auto-generated from " ^ file;
    "## *** DO NOT EDIT ***";
    "";
  ]
;;

(* same with cpp.ml *)
let make_source conf source filename content =
  let path = Filename.concat conf.Config.outdir filename in
  File_util.safe_open_out path (fun out ->
    let print = (fun s -> output_string out s; output_char out '\n') in
    let head = make_header_message source in
    List.iter print head;
    print_lines print content
  )
;;

let make_header conf source filename content = 
  make_source conf source filename content
;;
  

(* return : retval = self.client.call(names) *)
let gen_retval func args =
  (0, "retval = self.client.call('" ^ func ^ "', " ^ (String.concat ", " args) ^ ")")
;;

(* return : def func_name (self, args): *)
let gen_def func args =
  (0, "def " ^ func ^ " (self, " ^ (String.concat ", " args) ^ "):")
;;

let rec gen_type t name = match t with
    | Object -> raise (Unknown_type("Object is not supported"))
    | Bool | Int(_, _) | Float(_) | Raw | String -> 
	name
    | Struct s  -> s ^ ".from_msgpack(" ^ name ^ ")"
    | List t -> "[" ^ (gen_type t ("elim_" ^ name)) ^ " for " ^ ("elim_" ^ name) ^ " in " ^ name ^ "]"
    | Map(key, value) -> 
	let t1' = (gen_type key ("k_" ^ name)) in
	let t2' = (gen_type value ("v_" ^ name)) in
	  "{" ^ t1' ^ " : " ^ t2' ^ " for " ^ 
	    ("k_" ^ name)  ^ "," ^ ("v_" ^ name) ^ 
	    " in " ^ (name ^ ".items()") ^ "}"
    | Tuple [t1; t2] -> 
	"(" ^ gen_type t1 (name ^ "[0]") ^ "," ^ gen_type t2 (name ^ "[1]") ^")"
	  (* assert false (* TODO (tsushima): no example *) *)
    | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
    | Nullable(t) -> raise (Unknown_type "Nullable is not supported")

let gen_args args = 
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_arg_def f =
  (gen_type f.field_type "retval") ^ " " ^ f.field_name
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args ^ ";"
;;

let gen_ret_type = function
  | None -> "" (* TODO (tsushima): OK? or retval? *)
  | Some typ -> gen_type typ "retval"
;;

let gen_client_method m =
  let name = m.method_name in
  let args = name
    ::  List.map (fun f -> f.field_name) m.method_arguments in 
  let ret_type = gen_ret_type m.method_return_type in
  let call =
    [(gen_def (List.hd args) (List.tl args));
     (gen_retval (List.hd args) (List.tl args));
     (0, "return " ^ ret_type)] in
    call

let gen_client s =
  let methods = List.map gen_client_method s.service_methods in
  let constructor = [
    (0, "def __init__ (self, host, port):");
    (0, "address = msgpackrpc.Address(host, port)");
    (0, "self." ^ s.service_name ^ "= msgpackrpc.Client(address)")
  ] in
  let content = concat_blocks ((indent_lines 1 constructor) :: methods) in
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

(* TODO: rename*)
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

let number = ref (-1)
let rec gen_arg f =
  number := !number + 1;
  f (string_of_int (!number))

let rec gen_from_msgpack_types field_types =
  number := -1;
  let rec loop = function
    | [] -> []
    | [t] -> [(2, gen_type t (gen_arg (fun x -> "arg_" ^ x ^ "_")))]
    | t :: rest -> 
	let name = (gen_arg (fun x -> "arg_" ^ x ^ "_")) in
	(2, (gen_type t name) ^ ",") :: (loop rest) 
  in loop field_types
    
let gen_from_msgpack field_names field_types s =
  List.concat [
    [(0, "@staticmethod")];
    [(0, "def from_msgpack(arg):")];
    [(1, "return " ^ s ^ "(")];
    (gen_from_msgpack_types field_types);
    [(1, ")")]
  ]

let gen_message m =
(*   let fields = List.map gen_message_field m.message_fields in *)
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  List.concat [
    [
      (0, "class " ^ m.message_name ^ ":");
    ];
    indent_lines 1 [(gen_def "__init__" field_names)];
    indent_lines 2 (gen_self_without_comma field_names);
    [(0, "")];
    indent_lines 1 (gen_to_msgpack field_names);
    [(0, "")];
    indent_lines 1 (gen_from_msgpack field_names field_types m.message_name);
  ]
;;

let gen_typedef = function
  | Typedef(name, typ) ->
      [ (0, "typedef " ^ gen_type typ "retval" ^ " " ^ name ^ ";") ]
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
(*  let namespace = parse_namespace conf.Config.namespace in *)
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

(* same with cpp.ml *)
let get_services idl =
  let services =
    List.fold_left (fun lst x ->
      match x with
      | Service s ->
        s::lst
      | _ -> lst) [] idl in
  List.rev services
;;

let generate conf source idl =
  let services = get_services idl in
    gen_client_file conf source services;
    gen_type_file conf source idl 
;;
