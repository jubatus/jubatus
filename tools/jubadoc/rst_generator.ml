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

(*
let to_impl_strings = function
  | Service(_, methods,_) ->
    let to_keeper_string m =
      let Method(rettype, name, argv, decorators,_) = m in

      let rettype = Util.decl_type2string rettype in
      let sorted_argv = List.sort
	(fun lhs rhs -> let Field(l,_,_,_) = lhs in let Field(r,_,_,_) = rhs in l-r ) argv
      in
      let argv_strs = List.map
	(fun field -> let Field(_,t,n,_) = field in (decl_type2string t)^" "^n) sorted_argv
      in
      let argv_strs2 = List.map
	(fun field -> let Field(_,_,n,_) = field in n) (List.tl sorted_argv)
      in
    in 
    List.map to_keeper_string methods;
  | _ -> [];;
*)
let to_string = function
  | Typedef(name,t,doclines) -> "typedef"^name;
  | Enum(name,nums,doclines) -> "enum"^name;
  | Message(name,fields,doclines) -> "msg"^name;
  | Exception(name,fields,p,doclines) -> "ex"^name;
  | Service(name,methods,doclines) -> "service"^name;;

let generate s output strees =
  output <<< Generator.comment;

  List.iter (fun l -> output <<< (to_string l)) strees;
  ();;
