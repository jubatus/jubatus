
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

(*
class jubatus_module name_i classname_i include_i classimpl_i =
object (self)
  val name = name_i
  val classname = classname_i
  val includes = include_i
  val classimpl = classimpl_i
  val prototypes = match classimpl_i with |Stree.ClassImpl(_, proto, _) -> proto
  val members    = match classimpl_i with |Stree.ClassImpl(_, _, membs) -> membs
  val mutable output = stdout

  val back_rpc   = classname_i ^ "_back_rpc.hpp"
  val front_rpc  = classname_i ^ "_front_rpc.hpp"
  val client_h   = classname_i ^ "_client.hpp"
  val client_c   = classname_i ^ "_client.cpp"
  val keeper_h   = classname_i ^ "_keeper.hpp"
  val keeper_c   = classname_i ^ "_keeper.cpp"
  val server_h   = classname_i ^ "_server.hpp"
  val server_c   = classname_i ^ "_server.cpp"

  method generate_front_rpc =
    print_endline ("=================== " ^ front_rpc);
    output <<< make_file_begin "client" classname;
    output <<< (String.concat "" (List.map make_mprpc_decl prototypes));
    output <<< gen_mprpc_decl classname prototypes;
    output <<< make_file_end "client" classname;

  method generate_back_rpc =
    print_endline ("=================== " ^ back_rpc);
    output <<< make_file_begin "server" classname;
    output <<< (String.concat "" (List.map make_mprpc_decl prototypes));
    output <<< gen_mprpc_decl classname prototypes;
    output <<< make_file_end "server" classname;
    
  method generate_client = (* TODO: functor for multiple language generation *)
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
    output <<< include_dq [client_h; front_rpc];
    output <<< make_ns_begin "client" name;
(*    output <<< Client_template.constructor classname;
    output <<< Client_template.destructor classname;
    output <<< String.concat "\n" (List.map (Client_template.prototype2impl classname) prototypes); *)
    output <<< make_ns_end "client" name;

  method generate_keeper =
    print_endline ("=================== " ^ keeper_h);
    output <<< make_file_begin "keeper" name;
(*    output <<< Keeper_template.make_class_begin "jubakeeper";
    output <<< String.concat "\n" (List.map (Keeper_template.prototype2string "jubakeeper") prototypes); 
    output <<< Keeper_template.make_class_end "jubakeeper"; *)
    output <<< make_file_end "keeper" name;
    
    print_endline ("=================== " ^ keeper_c);
    output <<< include_dq [keeper_h; front_rpc; back_rpc; ];
    output <<< make_ns_begin "keeper" name;
(*    output <<< Keeper_template.constructor "jubakeeper";
    output <<< String.concat "\n" (List.map (Keeper_template.prototype2impl "jubakeeper") prototypes);
    output <<< Keeper_template.destructor "jubakeeper"; *)
    output <<< make_ns_end "keeper" name;

  method generate_server =
(*    let add_first_argv (t,n,argv,dir) = (t, n, (( Stree.find_anytype "string")::argv ), dir) in
    let server_prototypes = List.map add_first_argv prototypes in *)
    print_endline ("=================== " ^ server_h);
    output <<< make_file_begin "server" classname;
(*    output <<< Server_template.make_class_begin classname;
    output <<< String.concat "\n" (List.map (Server_template.prototype2string classname) server_prototypes);
    output <<< Server_template.make_class_end classname; *)
    output <<< make_file_end "server" classname;
    print_endline ("=================== " ^ server_c);
    output <<< include_dq [server_h; back_rpc ];
    output <<< make_ns_begin "server" classname;
(*    output <<< Server_template.constructor classname;
    output <<< Server_template.destructor classname;
    output <<< String.concat "\n" (List.map (Server_template.prototype2impl classname) server_prototypes); *)
    output <<< make_ns_end "server" classname;

  method generate =
(*    output <- stderr; *)
    self#generate_front_rpc;
    self#generate_back_rpc;
    self#generate_client;
    self#generate_keeper;
(*    self#generate_server; *)

end;;
*)
