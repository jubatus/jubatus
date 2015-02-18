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

open Util
open Stree

(*
let to_impl_strings = function
  | Service(_, methods,_) ->
    let to_proxy_string m =
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
    List.map to_proxy_string methods;
  | _ -> [];;
*)

let enum2string nums =
  let enumerate (i, s) =
    "- " ^ s ^ " = " ^ (string_of_int i)
  in
  String.concat "\n" (List.map enumerate nums);;


let to_string = function
  | Typedef(name,t,doclines) ->
    (* type newname = list<somemsg>
       =>
       .. describe:: type newname = list<somemsg>

       <doclines>
    *)
    ".. describe:: type " ^ name ^ " = " ^ (decl_type2mpidl t)
    ^ "\n" ^ (format_doclines doclines) ^ "\n";

  | Enum(name,nums,doclines) ->
    (* enum spam {
         0: ham
         1: foo
       }
       =>
       .. describe:: enum spam

       - ham = 0
       - foo = 1

       <doclines>
    *)
    ".. describe:: enum " ^ name
    ^ "\n\n"
    ^ (enum2string nums)
    ^ "\n\n"
    ^ (format_doclines doclines) ^ "\n";

  | Message(name,fields,doclines) ->
    (* message somemsg {
         #- aaa
         0: a
         #- bbbb bbb
         1: b
       }
       =>
       .. describe:: message

       + a
       
        - aaa in single line?

       + b

        - bbb
    *)
    ".. describe:: message " ^ name
    ^ "\n\n"
    ^ (String.concat "\n" (List.map field_type2mpidl fields))
    ^ "\n\n"
    ^ (format_doclines doclines) ^ "\n";

  | Exception(name,fields,p,doclines) ->
    let p_str = match p with
      | "" -> p;
      | _  -> " (< " ^ p ^ ")"
    in
    (*
      exception ex {
      }
    *)
    ".. describe:: " ^ name ^ p_str
    ^ "\n\n"
    ^ (String.concat "\n" (List.map field_type2mpidl fields))
    ^ "\n\n"
    ^ (format_doclines doclines) ^ "\n";

  | Service(name,methods,doclines) ->
    (*
      service s {
      }
      =>
      service s
      ---------

      .. describe:: list<int> foo(...)
    *)
    "service "^name^"\n"
    ^"----------------------\n\n"
    ^ (format_doclines doclines) ^ "\n\n"
    ^ (String.concat "\n" (List.map method_type2mpidl methods))
      
;;

let generate s output strees =
  output <<< (".. " ^ Generator.comment);
  output <<< "\n";

  output <<< "sometitle";
  output <<< "=========\n";
  List.iter (fun l -> output <<< (to_string l)) strees;
  ();;

