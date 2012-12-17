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

open Util
open Stree

let to_keeper_strings = function
  | Service(_, methods) ->
    let to_keeper_string (Method(rettype0, name, argv, decorators)) =
      let routing,rwtype,agg = Validator.make_decoration decorators in
      let rettype = Util.decl_type2string rettype0 in
      let sorted_argv = List.sort
	(fun lhs rhs -> let Field(l,_,_) = lhs in let Field(r,_,_) = rhs in l-r ) argv
      in
      let argv_types = List.map
	(fun field -> let Field(_,t,_) = field in t) (List.tl sorted_argv)
      in
      let argv_strs = List.map Util.decl_type2string argv_types in

      match routing with
	| Random ->
	  Printf.sprintf "    k.register_async_random<%s >(\"%s\"); //%s %s"
	    (String.concat ", " (rettype::argv_strs))  name
	    (Stree.aggtype_to_string agg) (Stree.reqtype_to_string rwtype);
	| Cht(i) -> (* when needs aggregator *)
	  let aggfunc =
	    let tmpl =
	      (* merge for map, concat for list *)
	      match rettype0 with (* FIXME: this should be fixed up as Util *)
		| Map(k,v) -> "<"^(Util.decl_type2string k)^","^(Util.decl_type2string v)^" >" ;
		| List t ->  "<"^(Util.decl_type2string t)^" >" ;
		| _ when agg = Pass ->  "<"^rettype^" >" ;
		| _ -> "" (* FIXME, dangerous *)
	    in
	    Printf.sprintf "pfi::lang::function<%s(%s,%s)>(&%s%s)" rettype rettype rettype
	      (Stree.aggtype_to_string agg) tmpl
	  in
	  Printf.sprintf "    k.register_async_cht<%d, %s >(\"%s\", %s); //%s" i
	    (String.concat ", " (rettype::(List.tl argv_strs))) name aggfunc
	    (Stree.reqtype_to_string rwtype)
	| Internal -> ""; (* no code generated in keeper *)
	| _ ->
	  let aggfunc =
	    let tmpl =
	      (* merge for map, concat for list *)
	      match rettype0 with
		| Map(k,v) -> "<"^(Util.decl_type2string k)^","^(Util.decl_type2string v)^" >" ;
		| List(t) ->  "<"^(Util.decl_type2string t)^" >" ;
		| _ when agg = Pass -> "<"^rettype^" >" ;
		| _ -> "" (* FIXME, dangerous *)
	    in
	    Printf.sprintf "pfi::lang::function<%s(%s,%s)>(&%s%s)" rettype rettype rettype
	      (Stree.aggtype_to_string agg) tmpl
	  in
	  Printf.sprintf "    k.register_async_%s<%s >(\"%s\", %s); //%s"
	    (Stree.routing_to_string routing)
	    (String.concat ", " (rettype::argv_strs))  name aggfunc
	    (Stree.reqtype_to_string rwtype)
    in
    List.map to_keeper_string methods;
  | _ -> [];;

let generate s output strees =
  output <<< Generator.comment;
  
  if s#internal then begin
    output <<< "#include \"../framework/keeper.hpp\"";
    output <<< "#include \"../framework/aggregators.hpp\"";
    output <<< "#include \"../common/exception.hpp\""
  end
  else begin
    output <<< "#include <jubatus/framework.hpp>";
    output <<< "#include <jubatus/framework/aggregators.hpp>";
    output <<< "#include <jubatus/common/exception.hpp>" 
  end;
  
  output <<< ("#include \""^s#basename^"_types.hpp\"");
  output <<< "#include <glog/logging.h>";
  output <<< ("using namespace "^s#namespace^";");
  output <<< "using namespace jubatus::framework;";
  output <<< "int main(int args, char** argv){";
  output <<< "  try{";
  output <<< "    keeper k(keeper_argv(args,argv,\""^s#basename^"\"));";

  List.iter (fun l -> output <<< l)
    (List.flatten (List.map to_keeper_strings
		     (List.filter Generator.is_service strees)));
  
  output <<< "    return k.run();";
  output <<< "  } catch (const jubatus::exception::jubatus_exception& e) {";
  output <<< "    LOG(FATAL) << e.diagnostic_information(true);";
  output <<< "    return -1;";
  output <<< "  }";
  output <<< "}";;

