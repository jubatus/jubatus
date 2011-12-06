
open Util

let make_mprpc_decl (retval,n,argv,decs,_,_) =
  "// " ^ (String.concat ", " decs) ^ "\n"
  ^ "MPRPC_PROC(" ^ n ^ "),\tresult<" ^ (Stree.to_string retval) ^ ">("
  ^ (String.concat ", " ("string" :: (List.map Stree.to_string argv))) ^ ");\n";;

let gen_mprpc_decl name prototypes =
  let get_name (_,n,_,_,_,_) = n in
  let names = List.map get_name prototypes in
  "MPRPC_GEN(1, " ^ (String.concat ", " (name :: names)) ^ "); ";;



class jubatus_module outdir_i name_i namespace_i typedefs_i structdefs_i classdefs_i =
object (self)
  val outdir = outdir_i
  val name = name_i
  val namespace = namespace_i
  val typedefs = typedefs_i
  val structdefs = structdefs_i
  val classdefs = classdefs_i
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
    let namespaces = [namespace; "server"] in
    output <<< include_dq [(name ^ "_server.hpp");
			   "server_util.hpp";
			   (name ^ "_serv.hpp")];
    output <<< include_b ["pficommon/lang/shared_ptr.h"];
    output <<< "\n";
    output <<< make_ns_begin namespaces;
    List.iter (fun c -> output <<< Server_template.make_class c) classdefs;
    output <<< make_ns_end namespaces;
    output <<< Server_template.make_main namespaces (List.hd classdefs);

  method generate_keeper =
    print_endline ("generate ==> " ^ keeper_c);
    output <<< "// this program is automatically generated. do not edit. ";
    output <<< Keeper_template.make_file_begin name;
    output <<< make_using_ns [namespace];
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
