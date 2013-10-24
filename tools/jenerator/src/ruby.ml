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

let gen_string_literal s =
  "\"" ^ String.escaped s ^ "\""
;;

let gen_bool_literal = function
  | true -> "true"
  | false -> "false"
;;

let gen_args args =
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args
;;

let gen_list vars =
  "[" ^ String.concat ", " vars ^ "]"
;;

let rec gen_type = function
  | Object -> "TObject.new"
  | Bool -> "TBool.new"
  | Int(signed, bits) -> gen_call "TInt.new"
    [gen_bool_literal signed; string_of_int bits]
  | Float(_) -> "TFloat.new"
  | Raw -> "TRaw.new"
  | String -> "TString.new"
  | Datum -> "TDatum.new"
  | Struct s  -> gen_call "TUserDef.new" [snake_to_upper s]
  | List t -> gen_call "TList.new" [gen_type t]
  | Map(key, value) -> gen_call "TMap.new" [gen_type key; gen_type value]
  | Nullable(t) -> gen_call "TNullable.new" [gen_type t]
;;

let gen_client_call m =
  let name = m.method_name in
  let args = List.map (fun f -> f.field_name) m.method_arguments in
  let arg_types = List.map (fun f -> f.field_type) m.method_arguments in

  let args' = gen_list args in
  let ret_type' = match m.method_return_type with
    | None -> "nil"
    | Some t -> gen_type t in
  let arg_types' = gen_list (List.map gen_type arg_types) in
  let call_arg = [gen_string_literal name; args'; ret_type'; arg_types'] in
  gen_call "@jubatus_client.call" call_arg
;;

(* return : def func_name (args): *)
let gen_def func = function
  | [] ->
    "def " ^ func
  | args ->
    "def " ^ func ^ "(" ^ (String.concat ", " args) ^ ")"
;;

let gen_client_method m =
  let name = m.method_name in
  let args = List.map (fun f -> f.field_name) m.method_arguments in
  [ (0, gen_def name args);
    (1,   gen_client_call m);
    (0, "end");
  ]
;;

let gen_client s =
  let constructor = [
    (0, "def initialize(host, port, name, timeout_sec=10)");
    (1,   "super");
    (0, "end");
    (0, "");
  ] in
  let methods = List.map gen_client_method s.service_methods in
  let content = concat_blocks (constructor :: methods) in
  List.concat [
    [
      (0, "class " ^ (snake_to_upper s.service_name) ^ " < Jubatus::Common::ClientBase");
      (1,   "include Jubatus::Common");
    ];
    indent_lines 1 content;
    [
      (0, "end")
    ]
  ]
;;


let gen_self_with_equal field_names =
  List.map (fun s -> (0, "@" ^ s ^ " = " ^ s)) field_names
;;

let gen_initialize field_names =
  List.concat [
    [ (0, gen_def "initialize" field_names) ];
    indent_lines 1 (gen_self_with_equal field_names);
    [ (0, "end") ]
  ]
;;
let gen_to_msgpack field_names field_types =
  let vars = List.map (fun v -> "@" ^ v) field_names in
  [
    (0, "def to_msgpack(out = '')");
    (1,   "t = " ^ gen_list vars);
    (1,   "return " ^ gen_call "TYPE.to_msgpack" ["t"]);
    (0, "end");
  ]
;;

let gen_message_type field_types =
  "TYPE = " ^ gen_call "TTuple.new" (List.map gen_type field_types)
;;

let gen_from_msgpack field_names field_types s =
  let s = snake_to_upper s in
  [
    (0, "def " ^ s ^ ".from_msgpack(m)");
    (1,  "val = TYPE.from_msgpack(m)");
    (1,  gen_call (s ^ ".new") ["*val"]);
    (0, "end")
  ]
;;

let gen_attr field_names =
  let fs = List.map ((^) ":") field_names in
  match fs with
    | [] -> []
    | lst -> [ (0, "attr_reader " ^ String.concat ", " lst)]
;;

let gen_str name field_names =
  List.concat [
    [
      (0, "def to_s");
      (1,   "gen = Jubatus::Common::MessageStringGenerator.new");
      (1,   gen_call "gen.open" [gen_string_literal name])
    ];
    List.map (fun f ->
      (1,   gen_call "gen.add" [gen_string_literal f; "@" ^ f])
    ) field_names;
    [
      (1,   "gen.close()");
      (1,   "return gen.to_s");
      (0, "end")
    ]
  ]
;;


let gen_message m =
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  concat_blocks [
    [
      (0, "class " ^ (snake_to_upper m.message_name));
      (1,   "include Jubatus::Common");
      (1,   gen_message_type field_types);
    ];
    (* def initiallize .. *)
    indent_lines 1 (gen_initialize field_names);
    (* def to_msgpack .. *)
    indent_lines 1 (gen_to_msgpack field_names field_types);
    (* def from_msgpack .. *)
    indent_lines 1 (gen_from_msgpack field_names field_types m.message_name);
    indent_lines 1 (gen_str m.message_name field_names);
    indent_lines 1 (gen_attr field_names);
    [
      (0, "end");
    ];
  ]
;;

let gen_typedef = function
  | Message m ->
      gen_message m
  | _ ->
    []
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = Filename.concat base "client.rb" in
  let clients = List.map (fun s ->
    let module_name = snake_to_upper s.service_name in
    concat_blocks [
      [
        (0, "module " ^ module_name);
        (0, "module Client");
      ];
      gen_client s;
      [
        (0, "end  # Client");
        (0, "end  # " ^ module_name);
      ];
    ]
  ) services in

  let content = concat_blocks [
    [
      (0, "require 'rubygems'");
      (0, "require 'msgpack/rpc'");
      (0, "require 'jubatus/common'");
      (0, "require File.join(File.dirname(__FILE__), 'types')");
    ];
    [
      (0, "module Jubatus");
    ];
    concat_blocks clients;
    [
      (0, "end  # Jubatus");
    ];
  ]
  in make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = Filename.concat base "types.rb" in
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "require 'rubygems'");
    (0, "require 'msgpack/rpc'");
    (0, "require 'jubatus/common'");
  ] in

  let content = concat_blocks [
    includes;
    [
      (0, "module Jubatus");
      (0, "module " ^ snake_to_upper base);
    ];
    concat_blocks types;
    [
      (0, "end  # " ^ snake_to_upper base);
      (0, "end  # Jubatus");
    ]
  ] in
  make_header conf source name content
;;

let generate conf source idl =
  let services = get_services idl in
  gen_client_file conf source services;
  gen_type_file conf source idl
;;
