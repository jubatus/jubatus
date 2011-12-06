(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.

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
    
