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

exception Not_class_impl

let make_class_begin classname =
  Printf.sprintf "class %s_impl_ : public %s<%s_impl_> \n{\n"
    classname classname classname;;

let make_class_end classname =
  Printf.sprintf "  pfi::lang::shared_ptr<%s_serv> p_;\n};\n" classname;;

let prototype2impl (t,n,argvs,decorators,is_const) =
(*  let is_cht = (List.mem "//@cht" decorators) in *)
  let do_routing = not (List.mem "//@fail_in_keeper" decorators) in
  let argvs_str =
    String.concat ", "
      (List.map (fun (i,argv)-> (Stree.to_string argv) ^ " arg" ^ string_of_int i)
	 (Util.add_index argvs))
  in
  let argvs_str2 m =
    let make_list b e =
      let rec make_list_ l i =
	if i = e then l
	else make_list_ (i::l) (i+1)
      in
      List.rev (make_list_ [] b)
    in
    if do_routing then
      String.concat ", " (List.map (fun i-> "arg" ^ string_of_int i) (make_list 1 m))
    else
      String.concat ", " (List.map (fun i-> "arg" ^ string_of_int i) (make_list 0 m))
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
    (Stree.to_string t) n argvs_str
    (String.concat " " decorators) str
  in
  make_impl (Printf.sprintf "%s %sp_->%s(%s);" (make_lock_statement is_const)
	       (make_return_statement t) n (argvs_str2 (List.length argvs)))

let memberdecl (t,n) =
  Printf.sprintf "  %s %s;\n" (Stree.to_string t) n;;

let make_class = function
  | Stree.ClassDef(classname, funcs, members) ->
    let use_cht =
      if List.exists Stree.include_cht_api funcs then "  p_->use_cht(); "
      else ""
    in
    make_class_begin classname
    ^ "public:\n"
    ^ Printf.sprintf "  %s_impl_(const server_argv& a)\n" classname
    ^ Printf.sprintf "    : %s<%s_impl_>(a.timeout),\n" classname classname
    ^ Printf.sprintf "      p_(new %s_serv(a))\n" classname
    ^ Printf.sprintf "  {  %s  };\n" use_cht
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "\n  int run(){ return p_->start(*this); };\n"
    ^ Printf.sprintf "\n  pfi::lang::shared_ptr<%s_serv> get_p(){ return p_; };\n" classname
    ^ "\nprivate:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";;

let make_main namespaces classdef =
  let namespace = String.concat "::" namespaces in
  let Stree.ClassDef(classname, _, _) = classdef in
  "int main(int args, char** argv){\n"
  ^ Printf.sprintf "  return jubatus::framework::run_server<%s::%s_impl_,%s::%s_serv>(args, argv);\n"
    namespace classname namespace classname
  ^ "}";;
