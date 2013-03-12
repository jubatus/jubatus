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
    "# This file is auto-generated from " ^ file;
    "# *** DO NOT EDIT ***";
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
  (0, "  @cli.call(:" ^ func ^ ", " ^ (String.concat ", " args) ^ ")")
;;

(* return : def func_name (self, args): *)
let gen_def func args =
  (0, "def " ^ func ^ "(" ^ (String.concat ", " args) ^ ")")
;;

let rec gen_type t name = match t with
    | Object -> raise (Unknown_type("Object is not supported"))
    | Bool | Int(_, _) | Float(_) | Raw | String -> 
	if name = "x" then "" else name
    | Struct s  -> s ^ ".from_msgpack(" ^ name ^ ")"
    | List t -> "[" ^ gen_type t name ^ "]"
    | Map(key, value) -> 
	".map {" ^ (gen_type key name) ^ " " ^ (gen_type value name) ^ "}" 
    | Tuple [t1; t2] -> 
	gen_type t1 (name ^ "[0]") ^ ", " ^ gen_type t2 (name ^ "[1]")
	  (* assert false (* TODO (tsushima): no example *) *)
    | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
    | Nullable(t) -> raise (Unknown_type "Nullable is not supported")


let gen_args args = 
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_arg_def f =
  (gen_type f.field_type "retval") ^ " " ^ f.field_name
;;

let gen_client_method m =
  let name = m.method_name in
  let args = name
    ::  List.map (fun f -> f.field_name) m.method_arguments in 
  let call =
    [(gen_def (List.hd args) (List.tl args));
     (gen_retval (List.hd args) (List.tl args));
     (0, "end")
    ] 
  in call

let gen_client s =
  let methods = List.map gen_client_method s.service_methods in
  let constructor = [
    (0, "def initialize(host, port)");
    (0, "  @cli = MessagePack::RPC::" ^ (String.capitalize s.service_name) ^ ".new(host, port)");
    (0, "end")
  ] in
  let content = concat_blocks (constructor :: methods) in
    List.concat [
      [
	(0, "class " ^ (String.capitalize s.service_name));
      ];
    indent_lines 1 content;
      [
	(0, "end")
      ]
    ]
;;

let gen_message_field f =
  (0, gen_arg_def f ^ ";")
;;

(* TODO: rename*)
let gen_self_with_comma field_names =
  (List.map (fun s -> (0, "@" ^ s ^ ",")) field_names)

let gen_self_without_comma field_names =
  (List.map (fun s -> (0, "@" ^ s ^ " = " ^ s)) field_names)

let gen_to_tuple field_names field_types =
  List.concat [
    [(0, "def to_tuple");
     (0, "[")];
     indent_lines 1 (List.map2 (fun s t -> (0, "@" ^ s ^ gen_type t "x" ^ ",")) field_names field_types);
    [(0, "]"); (0, "end")]]
    (* indent_lines 2 (gen_self_with_comma field_names); *)

let gen_to_msgpack field_names field_types =
  List.concat [
    gen_to_tuple field_names field_types;
    [(0, "def to_msgpack(out = '')");
     (1, "to_tuple.to_msgpack(out)");
     (0, "end")]
  ]


let number = ref (-1)
let rec gen_arg f =
  number := !number + 1;
  f (string_of_int (!number))

let rec gen_from_msgpack_types field_types =
  let rec loop field_types num = match field_types with
    | [] -> []
    | [t] -> 
	let name = "tuple[" ^ (string_of_int num) ^ "]" in
	[(2, gen_type t name)]
    | t :: rest -> 
	let name = "tuple[" ^ (string_of_int num) ^ "]" in
	(2, (gen_type t name) ^ ",") :: (loop rest (num + 1)) 
  in loop field_types 0
    
let gen_from_msgpack field_names field_types s =
  let s = (String.capitalize s) in
  List.concat [
    [(0, "def " ^ s ^ ".from_tuple(tuple)")];
    [(1, s ^ ".new(")];
    (gen_from_msgpack_types field_types);
    [(1, ")")];
    [(0, "end")]
  ]

let gen_attr_accessor field_names = 
  (1, ("attr_accessor " ^ (String.concat ", " (List.map (fun x -> ":" ^ x) field_names))))

let gen_message m =
(*   let fields = List.map gen_message_field m.message_fields in *)
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  List.concat [
    [
      (0, "class " ^ (String.capitalize m.message_name));
    ];
    indent_lines 1 [(gen_def "initialize" field_names)];
    indent_lines 2 (gen_self_without_comma field_names);
    [(1, "end")];
    [(0, "")];
    indent_lines 1 (gen_to_msgpack field_names field_types);
    [(0, "")];
    indent_lines 1 (gen_from_msgpack field_names field_types m.message_name);
    [gen_attr_accessor field_names;
     (0, "end");
     (0, "")];
  ]
;;

let gen_typedef = function
  | Typedef(name, typ) ->
      (* [ (0, "typedef " ^ gen_type typ "retval" ^ " " ^ name ^ ";") ] *)
      []
  | Message m ->
    gen_message m
  | _ ->
    []
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_client.rb" in
  let clients = List.map gen_client services in

  let content = concat_blocks [
    [
      (0, "require 'rubygems'");
      (0, "require 'msgpack/rpc'");
      (0, "require File.join(File.dirname(__FILE__), 'types')");
    ];
    List.concat [
      [(0, "module Jubatus")];
      (List.map (fun s -> (0, "module " ^ (String.capitalize s.service_name))) services);
      [(0, "module Client")]
    ];
    (concat_blocks clients);
    ((0, "end") :: (0, "end"):: (List.map (fun s -> (0, "end")) services));
  ]
  in make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = base ^ "_types.rb" in
(*  let namespace = parse_namespace conf.Config.namespace in *)
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "require 'rubygems'");
    (0, "require 'msgpack/rpc'")
  ] in

  let content = concat_blocks [
    includes;
    [(0, "module Jubatus");
     (0, ("module " ^ (String.capitalize base)))];
    (concat_blocks types);
    [(0, "end");
     (0, "end")]
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
