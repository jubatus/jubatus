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

let comment_out_head = "//"
;;

let make_header conf source filename content =
  make_source conf source filename content comment_out_head
;;

let type_files = ref [];;
let type_defs = ref [];;


(* rename : e.g., "rename_without_underbar" -> "RenameWithoutUnderbar" *) 
let rename_without_underbar st = 
  let rec loop str b = match str with
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

let rec include_list t = match t with
  | Map(key, value) ->
      (include_list key) && (include_list value)
  | List t -> true
  | Tuple [t1; t2] ->
      (include_list t1) && (include_list t2)
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
  | _ -> false
;;

let rec include_map t = match t with
  | Map(_) -> true
  | List(t) -> include_map t
  | Tuple [t1; t2] ->
      (include_map t1) && (include_map t2)
  | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
  | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
  | _ -> false
;;

let rec gen_type t = match t with
    | Object -> raise (Unknown_type("Object is not supported"))
    | Bool -> "boolean"
    | Int(_, n) -> if n <= 4 then "int" else "long" 
    | Float(false) -> "float"
    | Float(true) -> "double"
    | Raw -> "raw"
    | String -> "String"
    | Struct s  -> 
        (try (String.capitalize (gen_type (List.assoc s (!type_defs))))
         with Not_found -> (rename_without_underbar s))
    | List t -> 
        "List<" ^ (String.capitalize (gen_type t)) ^ " >"
    | Map(key, value) -> 
        "Map<" ^ (gen_type key) ^ ", " ^ (gen_type value) ^ " >"
    | Tuple [t1; t2] -> 
        type_files := (Tuple[t1; t2]) :: !type_files;
        "Tuple" ^ (String.capitalize (String.capitalize(gen_type t1))) ^ (String.capitalize (String.capitalize(gen_type t2)))
    | Tuple(ts) -> raise (Unknown_type "Tuple is not supported")
    | Nullable(t) -> raise (Unknown_type "Nullable is not supported")
;;

let gen_types_file name class_name t conf source = 
  let filename = (String.capitalize (rename_without_underbar name)) ^ ".java" in
  let header = List.concat [
    [(0, "package us.jubat." ^ class_name ^ ";");
     (0, "")];
    (if include_map t then [(0, "import java.util.Map;")]
     else []);
    (if include_list t then [(0, "import java.util.List;")]
     else []);
    [(0, "import org.msgpack.MessagePack;");
     (0, "import org.msgpack.annotation.Message;");
     (0, "")]
  ] in
  let content =
    (0, "@Message") :: 
      (gen_public_class name 
         (match t with
              Tuple[t1; t2] ->
                [(0, "public " ^ (gen_type t1) ^ " first;");
                 (0, "public " ^ (gen_type t2) ^ " second;")]
            | _ -> assert false)) in
    make_header conf source filename (header @ content)
;;

let gen_ret_type = function
  | None -> assert false (* TODO (tsushima): OK? *)
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

let gen_return args = match args with
  | [] -> assert false
  | [f] -> "return iface_." ^ f ^ ";"
  | f :: args -> "return iface_." ^ f ^ gen_args args ^ ";"
;;  

let gen_client_method m =
  let name = m.method_name in
  let args = List.map (fun f -> (f.field_name, f.field_type)) m.method_arguments in 
  let call =
    (gen_public m.method_return_type name args "" [(1, gen_return (name :: (List.map fst args)))]);
      in
    call
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
        (0, "public class " ^ (String.capitalize s.service_name) ^ "Client {");
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

let gen_message m class_name conf source =
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let field_types = List.map (fun f -> f.field_type) m.message_fields in
  let filename = (String.capitalize (rename_without_underbar m.message_name)) ^ ".java" in
  let header =
    List.concat
      [[(0, "package us.jubat." ^ class_name ^ ";");
        (0, "")];
       (if (List.exists include_map field_types) 
        then [(0, "import java.util.Map;")] else []);
       (if (List.exists include_list field_types) 
        then [(0, "import java.util.List;")] else []);
       [(0, "import org.msgpack.MessagePack;");
        (0, "import org.msgpack.annotation.Message;");
        (0, "")]
      ] in
  let content =
    (0, "@Message") :: 
      (gen_public_class (String.capitalize m.message_name)
         ((List.map2 (fun n t -> (0, "public " ^ (gen_type t) ^ " " ^ n ^ ";")) field_names field_types) @ 
            [(0, "");
             (0, "public " ^ (rename_without_underbar (String.capitalize m.message_name)) ^ "() {");
             (0, "");
             (0, "}")])) in
    make_header conf source filename (header @ content)
;;

let gen_typedef stat conf source = match stat with
  | Typedef(name, typ) ->
      type_defs := (name, typ) :: !type_defs
  | Message m ->
      let base = File_util.take_base source in
      gen_message m base conf source
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
  let filename = (String.capitalize base) ^ "Client.java" in
  let clients = List.map (fun x -> gen_client x (String.capitalize base)) services  in
  let content = concat_blocks [
    [(0, "");
     (0, "package us.jubat." ^ base ^ ";");
     (0, "")];
     (if (List.exists (map_search include_map) services) 
      then [(0, "import java.util.Map;")]
     else []);     
    (if (List.exists (map_search include_list) services) 
     then [(0, "import java.util.List;")] else []);
      [(0, "import org.msgpack.rpc.Client;");
       (0, "import org.msgpack.rpc.loop.EventLoop;")];
    (concat_blocks clients)
  ]
  in make_header conf source filename content
;;

let gen_typedef_file conf source idl =
  List.iter (fun stat -> gen_typedef stat conf source) idl
;;

let generate conf source idl =
  let services = get_services idl in
    gen_typedef_file conf source idl;
  let _ = gen_client_file conf source services in
  let base = File_util.take_base source in
    let tf = !type_files in
    List.iter (fun t -> gen_types_file (String.capitalize (gen_type t)) base t conf source) tf;
;;
