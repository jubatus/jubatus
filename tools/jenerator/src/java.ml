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

open Syntax
open Lib

module S =
  Set.Make
    (struct
      type t = decl_type * decl_type
      let compare = compare
     end)

let comment_out_head = "//"
;;

let make_header conf source filename content =
  make_source conf source filename content comment_out_head
;;

let type_files = ref S.empty;;
let type_defs = ref [];;

(* rename : e.g., "rename_without_underbar" -> "RenameWithoutUnderbar" *) 
let rename_without_underbar st = 
  let rec loop str b =
    match str with
    | "" -> ""
    | _ -> 
      let s = String.get str 0 in
      let str' = String.sub str 1 ((String.length str) - 1) in
      if s = '_' then (loop str' true)
      else if b then (String.capitalize (Char.escaped s)) ^ (loop str' false)
      else (Char.escaped s) ^ (loop str' false) in
  loop st true
;;


let gen_public_class name content =
  List.concat [
    [(0, "public class " ^ (rename_without_underbar name) ^ " {")];
    indent_lines 1 content;
    [(0, "};")]
  ]
;;

let rec include_list = function
  | Map(key, value) ->
    (include_list key) || (include_list value)
  | List t -> true
  | Tuple [t1; t2] ->
    (include_list t1) || (include_list t2)
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
  | _ -> false
;;

let rec include_map = function
  | Map(_) -> true
  | List(t) -> include_map t
  | Tuple [t1; t2] ->
    (include_map t1) || (include_map t2)
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
  | _ -> false
;;

let rec gen_type = function
  | Object -> raise (Unknown_type("Object is not supported"))
  | Bool -> "boolean"
  | Int(_, n) -> if n <= 4 then "int" else "long" 
  | Float(false) -> "float"
  | Float(true) -> "double"
  | Raw -> raise (Unknown_type("Raw is not supported"))
  | String -> "String"
  | Struct s  ->
    (try (gen_type (List.assoc s (!type_defs)))
     with Not_found -> (rename_without_underbar s))
  | List t -> 
    "List<" ^ gen_object_type t ^ " >"
  | Map(key, value) -> 
    "Map<" ^ gen_object_type key ^ ", " ^ gen_object_type value ^ " >"
  | Tuple [t1; t2] -> 
    type_files := S.add (t1, t2) (!type_files);
    "Tuple" ^ gen_object_type t1 ^ gen_object_type t2
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
and gen_object_type = function
  | Bool -> "Boolean"
  | Int(_, n) ->
    if n <= 4 then "Integer" else "Long"
  | Float(false) -> "Float"
  | Float(true) -> "Double"
  | t -> gen_type t
;;

let gen_package conf =
  if conf.Config.namespace == "" then
    []
  else
    [ (0, "package " ^ conf.Config.namespace ^ ";");
      (0, ""); ]
;;

let split_namespace conf =
  Str.split (Str.regexp "\\.") conf.Config.namespace
;;

let make_path conf filename =
  let package = split_namespace conf in
  File_util.concat_path (package @ [filename])
;;

let gen_types_file name t conf source = 
  let filename = rename_without_underbar name ^ ".java" in
  let path = make_path conf filename in
  let (t1, t2) = t in
  let t' = Tuple([t1; t2]) in
  let header = List.concat [
    gen_package conf;

    (if include_map t' then [(0, "import java.util.Map;")]
     else []);
    (if include_list t' then [(0, "import java.util.List;")]
     else []);
    [(0, "import org.msgpack.MessagePack;");
     (0, "import org.msgpack.annotation.Message;");
     (0, "")]
  ] in
  let content =
    (0, "@Message") :: 
      (gen_public_class name 
         [(0, "public " ^ (gen_type t1) ^ " first;");
          (0, "public " ^ (gen_type t2) ^ " second;")]) in
  make_header conf source path (header @ content)
;;

let gen_ret_type = function
  | None -> "void"
  | Some typ -> gen_type typ
;;

let gen_args args = 
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_args_with_type args = 
  "(" ^ String.concat ", " (List.map (fun (st, t) -> (gen_type t) ^ " "^ st) args) ^ ")"
;;

let gen_public ret_typ name args opt content = 
  List.concat
    [
      [(0, "public " ^ (gen_ret_type ret_typ) ^ " " ^ 
        name ^ (gen_args_with_type args) ^ opt ^ " {")];
      content;
      [(0, "}"); (0, "")]
    ]
;;

let gen_arg_def f =
  (gen_type f.field_type) ^ " " ^ f.field_name
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args ^ ";"
;;

let gen_return function_name args =
  "return iface_." ^ function_name ^ gen_args args ^ ";"
;;  

let gen_client_method m =
  let name = m.method_name in
  let args = List.map (fun f -> (f.field_name, f.field_type)) m.method_arguments in 
  let vars = List.map (fun f -> f.field_name) m.method_arguments in
  let call = match m.method_return_type with
    | None ->
      gen_call ("iface_." ^ name) vars
    | Some _ ->
      gen_return name vars
  in
  gen_public m.method_return_type name args "" [ (1, call) ]
;;

let gen_interface m = 
  let name = m.method_name in
  let args = List.map (fun f -> (f.field_name, f.field_type)) m.method_arguments in 
  let interface = (gen_ret_type m.method_return_type) ^ " " ^ name ^ (gen_args_with_type args) ^ ";" in
  interface
;;  

let gen_client s name =
  let constructor = [
    (0, "public " ^ name ^ "Client(String host, int port, double timeout_sec) throws Exception {");
    (1, "EventLoop loop = EventLoop.defaultEventLoop();"); 
    (1, "c_ = new Client(host, port, loop);");
    (1, "c_.setRequestTimeout((int) timeout_sec);");
    (1, "iface_ = c_.proxy(RPCInterface.class);");
    (0, "}");
    (0, "")
  ] in
  let interfaces = 
    List.concat [
      [(0, "public static interface RPCInterface {")];
      List.map (fun i -> (1, gen_interface i)) s.service_methods;
      [(0, "}"); (0, "")]
    ] in
  let methods = List.map gen_client_method s.service_methods in
  let content = List.concat methods in
  List.concat [
    [
      (0, "public class " ^ (rename_without_underbar s.service_name) ^ "Client {");
    ];
    indent_lines 1 constructor;
    indent_lines 1 interfaces;
    indent_lines 1 content;
    (indent_lines 1 
       [(0, "public Client get_client() {");
        (1, "return c_;");
        (0, "}");
        (0, "")
       ]);
    [(1, "private Client c_;");
     (1, "private RPCInterface iface_;");
     (0, "};")]
  ]
;;

let gen_message_field f =
  (0, "public " ^ gen_arg_def f ^ ";")
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

let gen_message m conf source =
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  let class_name = rename_without_underbar m.message_name in
  let filename = class_name ^ ".java" in
  let path = make_path conf filename in
  let header =
    List.concat
      [gen_package conf;

       (if (List.exists include_map field_types) 
        then [(0, "import java.util.Map;")] else []);
       (if (List.exists include_list field_types) 
        then [(0, "import java.util.List;")] else []);
       [(0, "import org.msgpack.MessagePack;");
        (0, "import org.msgpack.annotation.Message;");
        (0, "")]
      ] in
  let field_defs = List.map gen_message_field m.message_fields in
  let constructor = 
    [ (0, "");
      (0, "public " ^ class_name ^ "() {");
      (0, "}"); ] in
  let class_content = field_defs @ constructor in
  let content =
    (0, "@Message") :: 
      gen_public_class class_name class_content
  in
  make_header conf source path (header @ content)
;;

let gen_typedef stat conf source =
  match stat with
  | Typedef(name, typ) ->
    type_defs := (name, typ) :: !type_defs
  | Message m ->
    gen_message m conf source
  | _ ->
    ()
;;

let map_search' f m = 
  (List.exists (fun k -> f k.field_type) m.method_arguments) ||
    (match (m.method_return_type) with
    | None -> false
    | Some(t) -> f t)  
;;

let map_search f s =
  (List.exists (fun x -> map_search' f x) s.service_methods)
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = (rename_without_underbar base) ^ "Client.java" in
  let path = make_path conf filename in
  let clients = List.map (fun x -> gen_client x (rename_without_underbar base)) services  in
  let content = concat_blocks [
    gen_package conf;

    (if (List.exists (map_search include_map) services) 
     then [(0, "import java.util.Map;")]
     else []);     
    (if (List.exists (map_search include_list) services) 
     then [(0, "import java.util.List;")] else []);
    [(0, "import org.msgpack.rpc.Client;");
     (0, "import org.msgpack.rpc.loop.EventLoop;")];
    (concat_blocks clients)
  ]
  in make_header conf source path content
;;

let gen_typedef_file conf source idl =
  List.iter (fun stat -> gen_typedef stat conf source) idl
;;

let generate conf source idl =
  let services = get_services idl in
  gen_typedef_file conf source idl;
  let _ = gen_client_file conf source services in
  let tf = !type_files in
  S.iter (fun t -> 
    let (t1, t2) = t in
    gen_types_file (gen_type (Tuple([t1; t2]))) t conf source) tf
;;
