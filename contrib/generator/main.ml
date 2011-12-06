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

open Stree;;

let get_name filename =
  let start = try 1 + String.rindex filename '/' with _ -> 0 in
  let fin = String.rindex filename '.' in
  String.sub filename start (fin - start);;

let replace_suffix suffix filename =
  let base = String.sub filename 0 (String.length filename - 2) in
  base ^ "." ^ suffix;;

let outdir  = ref ".";;

 (* temporary error: to be fixed  *)
exception Multiple_argument_for_rpc of string
exception Multiple_decorator of string
exception Multiple_class
exception Wrong_cht_argv of string


let check_classdefs classdefs =
  let check_classdef classdef =
    let Stree.ClassDef(_,prototypes,_) = classdef in
    let check_prototype p = 
      let (_, n, argvs, decorators, _) = p in
      if ((List.mem "//@cht" decorators) && not (List.length argvs = 2)) then
	  raise (Wrong_cht_argv n)
      else if (not (List.mem "//@cht" decorators) && not (List.length argvs = 1)) then
	raise (Multiple_argument_for_rpc n)
      else if  not (List.length decorators = 1) then
	raise (Multiple_decorator n)
      else ()
    in
    List.iter check_prototype prototypes;
  in
  if not (List.length classdefs = 1) then raise Multiple_class
  else List.iter check_classdef classdefs;;
  

let add_method new_method classdef =
    let ClassDef(name, prototypes, members) = classdef in
    let new_prototypes = new_method :: (List.rev prototypes) in
    ClassDef(name, (List.rev new_prototypes), members);;

(* add a method of int(std::string, std::string) named method_name *)
let add_string_method method_name classdef =
  let new_method = (Stree.Int,
		    method_name,
		    [Stree.Other(Stree.Class("std::string"), true)],
		    ["//@broadcast"],
		    false) in
  add_method new_method classdef;;

let dope_routing classdef =
  let dope_routing_ prototype =
    let (t,n,argvs,decorators,is_const) = prototype in
    let new_argvs = Stree.Other(Stree.Class("std::string"), true) :: argvs in
    (t, n, new_argvs, decorators, is_const);
  in
  let ClassDef(name, prototypes, members) = classdef in
  ClassDef(name, (List.map dope_routing_ prototypes), members);;

let _ =
  Arg.parse [("-o", Arg.Set_string(outdir), "output directory")] (fun _ -> ()) "usage:";
  let source_file = Sys.argv.(1) in
  let lexbuf = Lexing.from_channel (open_in source_file) in
  try
    let (typedefs,structdefs,classdefs) = Parser.input Lexer.token lexbuf in

    let new_classdefs0 = List.map (add_method (Stree.Int, "save",
					       [Other(Stree.Class("std::string"), false)],
					       ["//@broadcast"], true)) classdefs
    in
    let new_classdefs1 = List.map (add_method (Stree.Int, "load",
					       [Other(Stree.Class("std::string"), false)],
					       ["//@broadcast"], false)) new_classdefs0
    in

    check_classdefs new_classdefs1;

    let new_classdefs2 = List.map dope_routing new_classdefs1 in

    let new_classdefs3 = List.map (add_method (Stree.Other(Stree.Class("std::string"), false),
					       "get_diff", [Stree.Int], ["//@fail_in_keeper"], true)) new_classdefs2
    in
    let new_classdefs4 = List.map (add_method (Stree.Int, "put_diff",
					       [Other(Stree.Class("std::string"), false)],
					       ["//@fail_in_keeper"], false)) new_classdefs3
    in
    
    List.iter Stree.print_classdef new_classdefs4;
(*    List.iter Stree.print_structdef structdefs;
    Stree.print_known_types();
    let new_classdefs1 = classdefs in *)

    let m = new Generator.jubatus_module (!outdir) (get_name source_file)
      "jubatus" (List.rev typedefs) (List.rev structdefs) (List.rev new_classdefs4) in
    m#generate;
    ()
  with
    | Parsing.Parse_error ->
      let p = Lexing.lexeme_start_p lexbuf in
      Printf.fprintf stderr
        "File \"%s\", line %d, character %d: syntax error.\n"
        p.Lexing.pos_fname p.Lexing.pos_lnum
        (p.Lexing.pos_cnum - p.Lexing.pos_bol)
    | exn ->
      print_endline (Printexc.to_string exn);;
