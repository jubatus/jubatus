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

open Stree

let output_endline out_channel str =
  output_string out_channel str;
  output_char out_channel '\n';;

let (<<<) = output_endline;;

let make_using_ns namespaces =
  Printf.sprintf "using namespace %s;\n" (String.concat "::" namespaces);;

let make_ns_begin namespaces =
  String.concat " " (List.map (fun namespace -> "namespace "^namespace^" {") namespaces)
  ^ "\n";;

let make_ns_end namespaces =
  (String.concat "" (List.map (fun _-> "}") namespaces))
  ^ " // " ^ (String.concat "::" namespaces);;

let make_header_header =
  "#pragma once\
";;

let include_b files =
  let f file = "#include <" ^ file ^ ">" in
  String.concat "\n" (List.map f files);;

let include_dq files =
  let f file = "#include \"" ^ file ^ "\"" in
  String.concat "\n" (List.map f files);;

let make_file_begin c namespace =
  "#pragma once\n"
  ^ "#include <vector>\n"
  ^ "#include <string>\n"
  ^ "using std::vector;\n"
  ^ "using std::string;\n" ^ make_ns_begin [c;namespace];;

let make_file_end = make_ns_end;;

let compose f g x = f (g x);;

let add_index list =
  let rec add_index_ i acc = function
    | [] -> acc;
    | hd::tl -> add_index_ (i+1) ((i, hd)::acc) tl
  in
  List.rev (add_index_ 0 [] list);;
    
let get_basename filename =
  let start = try 1 + String.rindex filename '/' with _ -> 0 in
  let fin = String.rindex filename '.' in
  String.sub filename start (fin - start);;

(* in C++ *)
let rec decl_type2string = function
  | Void    -> "void"
  | Object  -> "void*"
  | Bool    -> "bool"
  | Byte    -> "char"
  | Short   -> "short"
  | Int     -> "int"
  | Long    -> "long"
  | Ubyte   -> "unsigned char"
  | Ushort  -> "unsigned short"
  | Uint    -> "unsigned int"
  | Ulong   -> "unsigned long"
  | Float   -> "float"
  | Double  -> "double"
  | Raw     -> "char*"
  | String  -> "std::string"
  | Struct s -> s
  | List t   -> "std::vector<"^(decl_type2string t)^" >"
  | Map(k,v)  -> "std::map<"^(decl_type2string k)^","^(decl_type2string v)^" >"
  | Tuple ts when List.length ts == 2 ->
    "std::pair<"^(String.concat "," (List.map decl_type2string ts))^" >"
  | Tuple ts ->
    "msgpack::type::tuple<"^(String.concat "," (List.map decl_type2string ts))^" >"
  | Nullable t -> "/* nullable */" ^(decl_type2string t);;

let rec decl_type2string_ref = function
  | Void    -> "void"
  | Object  -> "void*"
  | Bool    -> "bool"
  | Byte    -> "char"
  | Short   -> "short"
  | Int     -> "int"
  | Long    -> "long"
  | Ubyte   -> "unsigned char"
  | Ushort  -> "unsigned short"
  | Uint    -> "unsigned int"
  | Ulong   -> "unsigned long"
  | Float   -> "float"
  | Double  -> "double"
  | Raw     -> "char*"
  | String  -> "std::string&"
  | Struct s -> s ^ "&"
  | List t   -> "std::vector<"^(decl_type2string t)^" >&"
  | Map(k,v)  -> "std::map<"^(decl_type2string k)^","^(decl_type2string v)^" >&"
  | Tuple ts when List.length ts == 2 ->
    "std::pair<"^(String.concat "," (List.map decl_type2string ts))^" >"
  | Tuple ts ->
    "msgpack::type::tuple<"^(String.concat "," (List.map decl_type2string ts))^" >&"
  | Nullable t -> "/* nullable */" ^(decl_type2string t);;


let rec decl_type2string_const_ref = function
  | Void    -> "void";
  | Object  -> "const void*";
  | Bool    -> "bool";
  | Byte    -> "char";
  | Short   -> "short";
  | Int     -> "int";
  | Long    -> "long";
  | Ubyte   -> "unsigned char";
  | Ushort  -> "unsigned short";
  | Uint    -> "unsigned int";
  | Ulong   -> "unsigned long";
  | Float   -> "float";
  | Double  -> "double";
  | Raw     -> "const char*";
  | String  -> "const std::string&";
  | Struct(s) -> "const "^ s ^ "&";
  | List(t)   -> "const std::vector<"^(decl_type2string t)^" >&";
  | Map(k,v)  -> "const std::map<"^(decl_type2string k)^","^(decl_type2string v)^" >&";
  | Tuple(ts) when List.length ts == 2 ->
    "std::pair<"^(String.concat "," (List.map decl_type2string ts))^" >";
  | Tuple(ts) ->
    "const msgpack::type::tuple<"^(String.concat "," (List.map decl_type2string ts))^" >&";
  | Nullable(t) -> "const /* nullable */" ^(decl_type2string t);;


(* todo: unify with Jubadoc_parser.mly:l90 a_type code. or do int using camlp4 *)
let rec decl_type2mpidl = function
  | Void -> "void";
  | Object -> "object";
  | Bool -> "bool";
  | Byte -> "byte";
  | Short -> "short";
  | Int -> "int";
  | Long -> "long";
  | Ubyte -> "ubyte";
  | Ushort -> "ushort";
  | Uint  -> "uint";
  | Ulong -> "ulong";
  | Float -> "float";
  | Double -> "double";
  | Raw  -> "raw";
  | String -> "string";
  | Struct(s) -> "message " ^ s;
  | List(decl_type_t) -> "list<"^(decl_type2mpidl decl_type_t)^">";
  | Map(k,v) -> "map<"^(decl_type2mpidl k)^", "^(decl_type2mpidl v)^">";
  | Tuple(decl_types) ->
    "tuple<" ^ (String.concat ", " (List.map decl_type2mpidl decl_types)) ^ ">";
  | Nullable(decl_type_t) -> "nullable "^(decl_type2mpidl decl_type_t);;
(*		 | `Null *)


let format_doclines doclines =
  let strip line =
    String.sub line 3 ((String.length line) - 3)
  in
  String.concat "" (List.map strip doclines);;

let field_type2mpidl (Field(i, t, n, c)) =
  let make_fielddoc = function
    | [] -> "";
    | doclines -> "\n\n - "
      ^ (String.concat " - " (List.map (fun line -> String.sub line 3 ((String.length line) - 3)) c))
  in
  "- " ^ (string_of_int i) ^ ": " ^ (decl_type2mpidl t) ^ " " ^ n
  ^ (make_fielddoc c);;

let method_type2mpidl (Method(t, n, fields, decs, cmt)) =
  let field2mpidl (Field(i, t, n, _)) =
    (string_of_int i) ^ ": " ^ (decl_type2mpidl t) ^ " " ^ n
  in
  let fields_str = String.concat ", " (List.map field2mpidl fields) in
  ".. describe:: " ^ (decl_type2mpidl t) ^ " " ^ n ^ "(" ^ fields_str ^ ")\n"
  ^ "\n"
  ^ (String.concat ", " (List.map decorator_to_string decs))
  ^ "\n\n"
  ^ (format_doclines cmt) ;;
