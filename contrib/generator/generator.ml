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

open Util

let make_mprpc_decl (retval,n,argv,decs,_,_) =
  "// " ^ (String.concat ", " decs) ^ "\n"
  ^ "MPRPC_PROC(" ^ n ^ "),\tresult<" ^ (Stree.to_string retval) ^ ">("
  ^ (String.concat ", " ("string" :: (List.map Stree.to_string argv))) ^ ");\n";;

let gen_mprpc_decl name prototypes =
  let get_name (_,n,_,_,_,_) = n in
  let names = List.map get_name prototypes in
  "MPRPC_GEN(1, " ^ (String.concat ", " (name :: names)) ^ "); ";;



class jubatus_module outdir_i name_i namespace_i typedefs_i structdefs_i classdefs_i internal_i =
object (self)
  val outdir = outdir_i
  val name = name_i
  val namespace = namespace_i
  val typedefs = typedefs_i
  val structdefs = structdefs_i
  val classdefs = classdefs_i
  val internal = internal_i
  val mutable output = stdout
  val mutable debugmode = false

  val idlfile  = name_i ^ ".idl"
  val server_c = name_i ^ "_impl.cpp"
  val keeper_c = name_i ^ "_keeper.cpp"

  method generate_idl =
    print_endline ("generate ==> " ^ idlfile);
    output <<< "# this idl is automatically generated. do not edit. ";
    List.iter (fun t -> output <<< Idl_template.make_typedef t) typedefs;
    List.iter (fun m -> output <<< Idl_template.make_message m) structdefs;
    List.iter (fun c -> output <<< Idl_template.make_service c) classdefs;
    
  method generate_impl =
    print_endline ("generate ==> " ^ server_c);
    output <<< "// this program is automatically generated. do not edit. ";
(*    output <<< include_dq ["server.hpp"; "../common/cmdline.h"]; *)
    output <<< include_dq [(name ^ "_server.hpp");
			   (name ^ "_serv.hpp")];
    output <<< include_b ["pficommon/lang/shared_ptr.h"];
    if internal then
      output <<< include_dq ["../framework.hpp"]
    else
      output <<< include_b ["jubatus/framework.hpp"];
    output <<< "\n";
    output <<< make_using_ns [namespace]; (* FIXME *)
    output <<< make_using_ns [namespace; "framework"]; (* FIXME *)
    let namespaces = [namespace; "server"] in
    output <<< make_ns_begin namespaces;
    List.iter (fun c -> output <<< Server_template.make_class c) classdefs;
    output <<< make_ns_end namespaces;
    output <<< Server_template.make_main namespaces (List.hd classdefs);

  method generate_keeper =
    print_endline ("generate ==> " ^ keeper_c);
    output <<< "// this program is automatically generated. do not edit. ";
    output <<< Keeper_template.make_file_begin name internal;
    output <<< make_using_ns [namespace];
    output <<< make_using_ns [namespace; "framework"];
    output <<< Keeper_template.make_main classdefs;
    output <<< Keeper_template.make_file_end name;

  method set_output filename =
    if debugmode then
      output <- stdout
    else
      output <- open_out filename

  method generate =
(*    debugmode <- true; *)
    self#set_output (String.concat "/" [outdir; idlfile]);
    self#generate_idl;
    self#set_output (String.concat "/" [outdir; server_c]);
    self#generate_impl;
    self#set_output (String.concat "/" [outdir; keeper_c]);
    self#generate_keeper;

end;;
