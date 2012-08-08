(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.

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

open Util
open Stree


let make_const_str = function
  | Update -> "";
  | Analysis ->" const";
  | Nolock -> " /* nolock!! */ ";;

let to_tmpl_strings = function
  | Service(_, methods) ->

    let to_tmpl_string (Method(rettype, name, argv, decorators)) =
      let routing,rwtype,_ = Validator.make_decoration decorators in
      let rettype = Util.decl_type2string rettype in
      let sorted_argv = List.sort
	(fun lhs rhs -> let Field(l,_,_) = lhs in let Field(r,_,_) = rhs in l-r ) argv
      in
      let argv_strs = List.map
	(fun field -> let Field(_,t,n) = field in (decl_type2string_const_ref t)^" "^n) (List.tl sorted_argv)
      in

      (Printf.sprintf "\n  %s %s(%s)%s; //%s %s"
	 rettype name (String.concat ", " argv_strs) (make_const_str rwtype)
	 (reqtype_to_string rwtype) (routing_to_string routing))
    in
    List.map to_tmpl_string methods;
  | _ -> [];;

let generate_header s output strees =
  output <<< "// this is automatically generated template header please implement and edit.";

  output <<< "#pragma once";
  if s#internal then
    output <<< "#include \"../framework.hpp\""
  else
    output <<< "#include <jubatus/framework.hpp>";
  output <<< ("#include \""^s#basename^"_types.hpp\"");

  output <<< "using namespace jubatus::framework;\n";
  output <<< "namespace jubatus { namespace server { // do not change";
  output <<< ("class "^s#basename^"_serv : public jubatus_serv // do not change");
  output <<< "{";
  output <<< "public:";
  output <<< ("  "^s#basename^"_serv(const server_argv& a); // do not change");
  output <<< ("  virtual ~"^s#basename^"_serv(); // do not change");

  List.iter (fun l -> output <<< l)
    (List.flatten (List.map to_tmpl_strings
		     (List.filter Generator.is_service strees)));

  output <<< "  void after_load();";
  output <<< "";
  output <<< "private:";
  output <<< "  // add user data here like: pfi::lang::shared_ptr<some_type> some_;";
  output <<< "};";
  output <<< "}} // namespace jubatus::server";;

let to_impl_strings classname = function
  | Service(_, methods) ->
    let to_impl_string (Method(rettype, name, argv, decorators)) =

      let routing,rwtype,_ = Validator.make_decoration decorators in
      let rettype = Util.decl_type2string rettype in
      let sorted_argv = List.sort
	(fun lhs rhs -> let Field(l,_,_) = lhs in let Field(r,_,_) = rhs in l-r ) argv
      in
      let argv_strs = List.map
	(fun field -> let Field(_,t,n) = field in (decl_type2string_const_ref t)^" "^n) (List.tl sorted_argv)
      in

      (Printf.sprintf "\n//%s, %s\n%s %s::%s(%s)%s\n{}"
	 (reqtype_to_string rwtype) (routing_to_string routing) rettype
	 classname name (String.concat ", " argv_strs) (make_const_str rwtype))
    in
    List.map to_impl_string methods;
  | _ -> [];;


let generate_impl s output strees =
  output <<< "// this is automatically generated template header please implement and edit.\n";

  let classname = s#basename^"_serv" in
  output <<< ("#include \""^classname^".hpp\"\n");

  output <<< "using namespace jubatus::framework;\n";
  output <<< "namespace jubatus { namespace server { // do not change";

  output <<< (classname^"::"^classname^"(const server_argv& a)");
  output <<< "  :framework::jubatus_serv(a)";
  output <<< "{";
  output <<< "  //somemixable* mi = new somemixable;";
  output <<< "  //somemixable_.set_model(mi);";
  output <<< "  //register_mixable(framework::mixable_cast(mi));";
  output <<< "}\n";

  output <<< (classname^"::~"^classname^"()");
  output <<< "{}\n";

  List.iter (fun l -> output <<< l)
    (List.flatten (List.map (to_impl_strings classname)
		     (List.filter Generator.is_service strees)));

  output <<< "";

  output <<< ("void "^classname^"::after_load(){}");

  output <<< "}} // namespace jubatus::server";;
