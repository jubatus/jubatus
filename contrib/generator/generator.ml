
open Util

let make_mprpc_decl (retval,n,argv,decs,_,_) =
  "// " ^ (String.concat ", " decs) ^ "\n"
  ^ "MPRPC_PROC(" ^ n ^ "),\tresult<" ^ (Stree.to_string retval) ^ ">("
  ^ (String.concat ", " ("string" :: (List.map Stree.to_string argv))) ^ ");\n";;

let gen_mprpc_decl name prototypes =
  let get_name (_,n,_,_,_,_) = n in
  let names = List.map get_name prototypes in
  "MPRPC_GEN(1, " ^ (String.concat ", " (name :: names)) ^ "); ";;

(* "jubatus" "sample" [prototype] => [source code filename] *)
(* string -> string -> prototypes list -> string list *)

class jubatus_module name_i namespace_i includes_i classimpls_i =
object (self)
  val name = name_i
  val namespace = namespace_i
  val includes = includes_i
  val classimpls = classimpls_i
  val mutable output = stdout

  val client_h  = name_i ^ "_client.hpp"
  val client_c  = name_i ^ "_client.cpp"
  val keeper_h  = name_i ^ "_keeper.hpp"
  val keeper_c  = name_i ^ "_keeper.cpp"
  val server_h  = name_i ^ "_server.hpp"
  val server_c  = name_i ^ "_server.cpp"

(*  method generate_front_rpc =
    print_endline ("=================== " ^ front_rpc);
    output <<< make_file_begin "client" classname;
    output <<< (String.concat "" (List.map make_mprpc_decl prototypes));
    output <<< gen_mprpc_decl classname prototypes;
    output <<< make_file_end "client" classname; *)
(*
  method generate_back_rpc =
    print_endline ("=================== " ^ back_rpc);
    output <<< make_file_begin "server" classname;
    output <<< (String.concat "" (List.map make_mprpc_decl prototypes));
    output <<< gen_mprpc_decl classname prototypes;
    output <<< make_file_end "server" classname;
*)  
(*  method generate_client = (* TODO: functor for multiple language generation *)
    print_endline ("=================== " ^ client_h);
    output <<< make_header_header;
    output <<< include_b ["vector"; "string"];
    output <<< make_ns_begin "client" name;
    (*    output <<< Client_template.make_class_begin classname;
	  output <<< String.concat "\n" (List.map (Client_template.prototype2string classname) prototypes);
	  output <<< Client_template.make_class_end classname; *)
    output <<< make_ns_end "client" name;
    print_endline ("=================== " ^ client_c);
    output <<< include_b ["pficommon/math/random.h"];
    output <<< include_dq [client_h];
    output <<< make_ns_begin "client" name;
    (*    output <<< Client_template.constructor classname;
	  output <<< Client_template.destructor classname;
	  output <<< String.concat "\n" (List.map (Client_template.prototype2impl classname) prototypes); *)
    output <<< make_ns_end "client" name;
*)

  method generate_server =
    print_endline ("==" ^ server_c ^ "==");
    output <<< include_b  includes;
    output <<< include_dq ["server.hpp"; "../common/cmdline.h"];
    let namespaces = [namespace; "server"] in
    output <<< make_ns_begin namespaces;
    List.iter (fun c -> output <<< Server_template.make_class c) classimpls;
    output <<< make_ns_end namespaces;
    output <<< Server_template.make_main ();

  method generate_keeper =
(*    output <<< make_file_begin "keeper" name;
    output <<< Keeper_template.make_class_begin "jubakeeper";
    output <<< String.concat "\n" (List.map (Keeper_template.prototype2string "jubakeeper") prototypes); 
    output <<< Keeper_template.make_class_end "jubakeeper";
    output <<< make_file_end "keeper" name;
    print_endline ("==" ^ keeper_h ^ "==");
    print_endline ("==" ^ keeper_c ^ "=="); *)
(*    output <<< include_dq [keeper_h; front_rpc; back_rpc; ]; *)
    output <<< make_ns_begin [namespace; "keeper"];
(*    output <<< Keeper_template.constructor "jubakeeper";
    output <<< String.concat "\n" (List.map (Keeper_template.prototype2impl "jubakeeper") prototypes);
    output <<< Keeper_template.destructor "jubakeeper"; *)
    output <<< make_ns_end [namespace; "keeper"];

  method generate =
    output <- stdout;
(*    self#generate_front_rpc;
    self#generate_back_rpc; 
    self#generate_client; *)
    self#generate_server;
(*    self#generate_keeper; *)

end;;
