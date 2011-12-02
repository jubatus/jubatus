
let make_service_begin service_name =
  Printf.sprintf "service %s :1 {\n" service_name;;

let make_service_end _ =
  "}";;

let rec to_idl_typestr = function
  | Stree.Void -> "void";
  | Stree.Int  -> "int";
  | Stree.Char -> "char";
  | Stree.Double -> "double";
  | Stree.Other(ud, is_ref) ->
    let rec ud_type2string = function
      | Stree.Class(s) when s = "std::string" -> "string";
      | Stree.Class(s) -> s;
      | Stree.Struct(s) -> s;
      | Stree.Template(v, l) when v = "vector" or v = "std::vector" -> (* FIXME! *)
	let list = List.map to_idl_typestr l in
	"list<" ^ (String.concat ", " list) ^ "> " ;
      | Stree.Template(v, l) when v = "map" or v = "std::map" -> (* FIXME! *)
	let list = List.map to_idl_typestr l in
	"map<" ^ (String.concat ", " list) ^ "> " ;
      | Stree.Template(v, l) when v = "pair" or v = "std::pair" -> (* FIXME! *)
	let list = List.map to_idl_typestr l in
	"pair<" ^ (String.concat ", " list) ^ "> " ;
      | Stree.Template(n, l) ->
	let list = List.map to_idl_typestr l in
	n ^ "<" ^ (String.concat ", " list) ^ "> " ;
(*      | Stree.Namespace(ns, ud) -> ns ^ "::" ^ (ud_type2string ud) *)
    in
    ud_type2string ud;
  | Stree.Destructor -> "";
  | Stree.Constructor -> "";;


let make_argv argvs =
  let rec make_argv_ str_list i = function
    | [] -> str_list ;
    | hd::tl ->
      make_argv_ ((Printf.sprintf "%d: %s arg%d" i (to_idl_typestr hd) i) :: str_list ) (i+1) tl
  in
  let l = make_argv_ [] 0 argvs in
  String.concat ", " (List.rev l);;

let prototype2string (t,n,argvs,decorators,_) =
  Printf.sprintf "  %s %s(%s) # %s \n"
    (to_idl_typestr t) n 
    (make_argv argvs)
    (String.concat " " decorators);;

let make_service = function
  | Stree.ClassDef(classname, funcs, members) ->
    let filter (_,n,_,_,_) =
      not ((classname = n) or ("~"^classname) = n)
    in
    let filtered = List.filter filter funcs in
    make_service_begin classname
    ^ (String.concat "" (List.map prototype2string filtered))
    ^ make_service_end classname;;


let make_member members =
  let rec make_member_ str_list i = function
    | [] -> str_list ;
    | (t,n)::tl ->
      make_member_ ((Printf.sprintf "  %d: %s %s" i (to_idl_typestr t) n) :: str_list ) (i+1) tl
  in
  let l = make_member_ [] 0 members in
  String.concat "\n" (List.rev l);;

let make_message_begin structname =
  Printf.sprintf "message %s {\n" structname;;

let make_message_end structname =
  "}\n";;

let make_message = function
  | Stree.StructDef(structname, members) ->
    make_message_begin structname
    ^ make_member members
    ^ "\n"
    ^ make_message_end structname;;

let make_typedef = function
  | Stree.TypeDef(t, name) ->
    Printf.sprintf "type %s = %s\n" name (to_idl_typestr t);;
