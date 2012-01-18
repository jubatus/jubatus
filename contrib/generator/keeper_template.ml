(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

let make_file_begin name =
  "#include \"keeper.hpp\"\n"
  ^Printf.sprintf "#include \"%s_types.hpp\"\n" name
;;

let make_file_end _ = "";;

let rec to_string = function
  | Stree.Void -> "void";
  | Stree.Int  -> "int";
  | Stree.Char -> "char";
  | Stree.Double -> "double";
  | Stree.Other(ud, is_ref) ->
    let rec ud_type2string = function
      | Stree.Class(s) -> s;
      | Stree.Struct(s) -> s;
      | Stree.Template(n, l) when (n = "tuple" or n = "std::tuple") && ((List.length l) = 2) ->
	let list = List.map to_string l in
	"std::pair<" ^ (String.concat ", " list) ^ "> " ;
      | Stree.Template(n, l) ->
	let list = List.map to_string l in
	n ^ "<" ^ (String.concat ", " list) ^ "> " ;
(*      | Namespace(ns, ud) -> ns ^ "::" ^ (ud_type2string ud) *)
    in
    ud_type2string ud;
  | Stree.Destructor -> "";
  | Stree.Constructor -> "";;


let generate_class_keeper clazz =
  let Stree.ClassDef(_, prototypes, _) = clazz in
  let prototype2register p =
    let const2type = function
      | true -> "analysis";
      | false -> "update";
    in
    let decorators2str decorators =
      let decorator2str str =
	String.sub str 3 ((String.length str) - 3)
      in
      List.fold_left (fun  _ d -> decorator2str d) "" decorators
    in
    let make_rettype is_const rettype =
      if is_const then (Stree.to_string rettype) ^ ", "
      else ""
    in
    let make_argtypes argtypes decorators =
      if List.mem "//@cht" decorators then
	List.tl (List.tl argtypes)
      else
	List.tl argtypes
    in
    let (rettype, name, argtypes, decorators, is_const) = p in
    Printf.sprintf "  k.register_%s_%s<%s%s>(\"%s\");"
      (decorators2str decorators) (const2type is_const) (make_rettype is_const rettype)
      (String.concat ", " (List.map to_string (make_argtypes argtypes decorators))) name
  in
  let filter (_,_,_,decorators,_) = not (List.mem "//@fail_in_keeper" decorators) in
  String.concat "\n" (List.map prototype2register (List.filter filter prototypes));;

(*  | _ -> raise Stree.Unkown_directive;; *)
let make_main classdefs =
  "int main(int args, char** argv){\n"
  ^ "  keeper k(keeper_argv(args, argv));\n"
  ^ String.concat "\n" (List.map generate_class_keeper classdefs)
  ^ "\n  k.run();"
  ^ "\n}";;
