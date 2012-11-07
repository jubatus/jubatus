(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.

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
