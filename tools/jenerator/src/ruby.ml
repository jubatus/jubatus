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

let make_header conf source filename content = 
  make_source conf source filename content comment_out_head
;;

(* return : retval = @cli.call(names) *)
let gen_retval' = function
  | [] -> assert false
      (* Because m.method_name (@gen_client_method) can not be empty *)
  | func :: [] ->
      "@cli.call(:" ^ func ^ ")"
  | func :: args ->
      "@cli.call(:" ^ func ^ ", " ^ (String.concat ", " args) ^ ")"
;;

let gen_retval args typ = match typ with
  | Some(t) -> 
      (match t with
      | Struct (st) ->
        (String.capitalize st) ^ ".from_tuple(" ^ gen_retval' args ^ ")"
      | Bool | Float(_) | String | Map(_) | List(_) -> gen_retval' args
      | _ -> gen_retval' args
      (* TODO: OK? *)
      )      
  | None -> gen_retval' args
;;

(* return : def func_name (args): *)
let gen_def = function
  | [] -> assert false
  | func :: [] ->
      "def " ^ func 
  | func :: args ->
      "def " ^ func ^ "(" ^ (String.concat ", " args) ^ ")"
;;

let rec gen_type t name = match t with
  | Object -> raise (Unknown_type("Object is not supported"))
  | Bool | Int(_, _) | Float(_) | Raw | String -> 
      name
  | Struct s  -> (String.capitalize s) ^ ".from_tuple(" ^ name ^ ")"
  | List t -> 
      (match t with
      | Bool | Int(_, _) | Float(_) | Raw | String -> name ^ ".map {|x| x}"
      | _ ->
        name ^ ".map {|x|  [" ^ gen_type t "x" ^ "] }")
  | Map(key, value) -> 
      name ^ ".each_with_object({}) {|(k,v),h| h[k] = v}" (* TODO: OK? *)
  | Tuple [t1; t2] ->
      gen_type t1 (name ^ "[0]") ^ ", " ^ gen_type t2 (name ^ "[1], ")
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
;;

let gen_arg_def f =
  (gen_type f.field_type "retval") ^ " " ^ f.field_name
;;

let gen_client_method m =
  let name = m.method_name in
  let args = name ::  List.map (fun f -> f.field_name) m.method_arguments in 
  let call =
    [(0, gen_def args);
     (1, gen_retval args m.method_return_type);
     (0, "end")
    ] 
  in call
;;

let gen_client s =
  let constructor = [
    (0, "def initialize(host, port)");
    (1, "@cli = MessagePack::RPC::Client.new(host, port)");
    (0, "end");
    (0, "def get_client");
    (1, "@cli");
    (0, "end")
  ] in
  let methods = List.map gen_client_method s.service_methods in
  let content = List.concat (constructor :: methods) in
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


let gen_self_with_comma field_names =
  (List.map (fun s -> (0, "@" ^ s ^ ",")) field_names)
;;

let gen_self_with_equal field_names =
  (List.map (fun s -> (0, " @" ^ s ^ " = " ^ s ^ " ")) field_names)
;;

let gen_initialize field_names = 
    (List.concat [[(0, gen_def ("initialize"::field_names))];
                  indent_lines 1 (gen_self_with_equal field_names);
                  [(0, "end")]])
;;

(* ad hoc ..  TODO: nested struct *)
let gen_type' t name = match t with
  | Struct s  -> name ^ ".to_tuple"
  | _ -> gen_type t name
;;

let gen_to_tuple' field_names field_types = 
  let rec loop s t = match (s, t) with
    | (s :: [], t :: []) -> (0, gen_type' t (" @" ^ s)) :: [(0, "]")]
    | (s :: ss, t :: ts) -> (0, (gen_type' t (" @" ^ s)) ^ ",") :: (loop ss ts)
    | _ -> assert false in
    (0, "[") :: (loop field_names field_types)
;;


let gen_to_tuple field_names field_types =
  List.concat [
    [(0, "def to_tuple")];
    indent_lines 1 (gen_to_tuple' field_names field_types);
    [(0, "end")]]
;;

let gen_to_msgpack field_names field_types =
  [(0, "def to_msgpack(out = '')");
   (1, "to_tuple.to_msgpack(out)");
   (0, "end")]
;;

let rec gen_from_tuple_types field_types =
  let rec loop field_types num = match field_types with
    | [] -> []
    | [t] -> 
      let name = "tuple[" ^ (string_of_int num) ^ "]" in
      [(2, gen_type t name)]
    | t :: rest -> 
      let name = "tuple[" ^ (string_of_int num) ^ "]" in
      (2, (gen_type t name) ^ ",") :: (loop rest (num + 1)) 
  in loop field_types 0
;;
    
let gen_from_tuple field_names field_types s =
  let s = (String.capitalize s) in
  List.concat [
    [(0, "def " ^ s ^ ".from_tuple(tuple)")];
    [(1, s ^ ".new(")];
    (gen_from_tuple_types field_types);
    [(1, ")")];
    [(0, "end")]
  ]
;;

let gen_attr_reader field_names field_types = 
  let rec loop field_names field_types = match (field_names, field_types) with
    | ([], []) -> []
    | (n :: ns, t :: ts) ->
      (match t with
      | Map(_) | Struct(_) -> (":" ^ n) :: (loop ns ts)
      | _ -> (loop ns ts))
    | _ -> assert false
  in
    match (loop field_names field_types) with
      | [] -> []
      | lst -> [(0, "attr_reader " ^ (String.concat ", " lst))]
;;
  

let gen_attr_accessor field_names field_types = 
  let rec loop field_names field_types = match (field_names, field_types) with
    | ([], []) -> []
    | (n :: ns, t :: ts) ->
      (match t with
      | Map(_) | Struct(_) ->(loop ns ts)
      | _ ->  (":" ^ n) :: (loop ns ts))
    | _ -> assert false
  in
    match (loop field_names field_types) with
      | [] -> []
      | lst -> [(0, "attr_accessor " ^ (String.concat ", " lst))]  
;;

let gen_attr field_names field_types = 
  (gen_attr_reader field_names field_types) @
    (gen_attr_accessor field_names field_types)
;;

let gen_message m =
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  List.concat [
    [
      (0, "class " ^ (String.capitalize m.message_name));
    ];
    (* def initiallize .. *)
    indent_lines 1 (gen_initialize field_names);
    (* def to_tuple .. *)
    indent_lines 1 (gen_to_tuple field_names field_types);
    (* def to_msgpack .. *)
    indent_lines 1 (gen_to_msgpack field_names field_types);
    (* def from_tuple .. *)
    indent_lines 1 (gen_from_tuple field_names field_types m.message_name);
    indent_lines 1 (gen_attr field_names field_types);
    [(0, "end");
     (0, "")];
  ]
;;

let gen_typedef' name typ = 
  [
    (0, "class " ^ (String.capitalize name));
    (1, gen_def [(String.capitalize name) ^ ".from_tuple"; "tuple"]);
    (2, (gen_type typ "tuple"));
    (1, "end");
    (1, gen_def ["to_tuple"; "o"]);
    (2, "o");
    (1, "end");
    (0, "end")
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
    ((0, "end") :: 
     (0, "end") :: 
     (List.map (fun s -> (0, "end")) services));
  ]
  in make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = base ^ "_types.rb" in
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "require 'rubygems'");
    (0, "require 'msgpack/rpc'");
    (0, "module Jubatus");
    (0, ("module " ^ (String.capitalize base)))
  ] in

  let content = concat_blocks [
    includes;
    (concat_blocks types);
    [(0, "end"); (* for "module Jubatus" *)
     (0, "end")] (* for "module " ^ (String.capitalize base) *)
  ] in
  make_header conf source name content
;;

let generate conf source idl =
  let services = get_services idl in
    gen_client_file conf source services;
    gen_type_file conf source idl 
;;
