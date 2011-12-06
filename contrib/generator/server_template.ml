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

exception Not_class_impl

let make_class_begin classname =
  Printf.sprintf "class %s_impl_ : public %s<%s_impl_> \n{\n"
    classname classname classname;;

let make_class_end classname =
  Printf.sprintf "  pfi::lang::shared_ptr<%s_serv> p_;\n};\n" classname;;

let prototype2impl (t,n,argvs,decorators,is_const) =
  let do_routing = not (List.mem "//@fail_in_keeper" decorators) in
  let rec argvs_str str i = function
    | [] -> String.concat ", " (List.rev str);
    | hd::tl -> argvs_str (((Stree.to_string hd) ^ " arg" ^ (string_of_int i)) :: str) (i+1) tl
  in
  let argvs_str2 n =
    let rec make_list l = function
      | 0 -> l
      | i -> make_list ((n-i)::l) (i-1)
    in
    if do_routing then
      String.concat ", " (List.map (fun i-> "arg" ^ string_of_int i) (make_list [] (n-1)))
    else
      String.concat ", " (List.map (fun i-> "arg" ^ string_of_int i) (make_list [] n))
  in
  let make_return_statement = function
    | Stree.Void -> "";
    | _ -> "return "
  in
  let make_lock_statement = function
    | true -> "JRLOCK__(p_);";
    | false -> "JWLOCK__(p_);"
  in
  let make_impl str = Printf.sprintf "  %s %s(%s) %s\n  { %s };\n"
    (Stree.to_string t) n (argvs_str [] 0 argvs) (String.concat " " decorators) str
  in
  if do_routing then
    make_impl (Printf.sprintf "%s %sp_->%s(%s);" (make_lock_statement is_const)
		 (make_return_statement t) n (argvs_str2 (List.length argvs)))
  else
    Printf.sprintf "#ifdef HAVE_ZOOKEEPER_H\n%s#endif\n"
      (make_impl (Printf.sprintf "%s %sp_->%s_impl(%s);" (make_lock_statement is_const)
		    (make_return_statement t) n (argvs_str2 (List.length argvs))))


let memberdecl (t,n) =
  Printf.sprintf "  %s %s;\n" (Stree.to_string t) n;;

let make_class = function
  | Stree.ClassDef(classname, funcs, members) ->
    make_class_begin classname
    ^ "public:\n"
    ^ Printf.sprintf "  %s_impl_(int args, char** argv)\n" classname
    ^ Printf.sprintf "    : p_(new %s_serv(args, argv)){};\n" classname
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "\n  int run(){ return p_->start(this); };\n"
    ^ "\nprivate:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";;

let make_main namespaces classdef =
  let namespace = String.concat "::" namespaces in
  let Stree.ClassDef(classname, _, _) = classdef in
  "int main(int args, char** argv){\n"
  ^ Printf.sprintf "  return jubatus::run_server<%s::%s_impl_>(args, argv);\n"
    namespace classname
  ^ "}";;
