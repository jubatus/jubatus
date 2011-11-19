
type user_defined_type = Class of string
			 | Template  of string * (anytype list)
			 | Namespace of string * user_defined_type  (* currently no namespace is implemented *)

and  anytype = Void | Int | Char | Double
	       | Other of user_defined_type

type decorator = string
type prototype = anytype * string * ((anytype * string) list) * (decorator list) * string * bool (* is_const *)
type member    = anytype * string (* static is not allowed, const is not implemented *)

(* currently all members should be public, and 'public:' and 'private:' notations are ignored *)
type class_impl = ClassImpl of string * (prototype list) * (member list)

exception Unkown_directive

let known_types = Hashtbl.create 256;;

let _ =
  List.iter (fun (k,v) -> Hashtbl.replace known_types k v) [
    "int", Int;
    "void", Void;
    "char", Char;
    "double", Double;
  ];;

let symbols = Hashtbl.create 256

let _ =
  List.iter (fun (k,v) -> Hashtbl.replace symbols k v) [
    "", "";
  ];;  

let add_known_types name =
  Hashtbl.replace known_types name (Other(Class(name)));;

let make_template t argv =
  let newtype = Other( Template(t, argv) ) in
  Hashtbl.replace known_types t newtype;
  newtype;;

let make_anytype t =
  try
    Hashtbl.find known_types t
  with
      Not_found ->
	let newtype = Other(Class(t)) in
	Hashtbl.replace known_types t newtype;
	newtype;;

let find_anytype t =
  Hashtbl.find known_types t

let make_symbol s =
  Hashtbl.replace symbols s s;
  s;;

let rec to_string = function
  | Void -> "void";
  | Int  -> "int";
  | Char -> "char";
  | Double -> "double";
  | Other(ud) ->
    let rec ud_type2string = function
      | Class(s) -> s;
      | Template(n, l) ->
	let list = List.map to_string l in
	n ^ "<" ^ (String.concat ", " list) ^ "> " ;
      | Namespace(ns, ud) -> ns ^ "::" ^ (ud_type2string ud)
    in
    ud_type2string ud

let print_all()=
  print_string "\nknown types: ";
  let p_ k _ = print_string (k ^ ", " ) in Hashtbl.iter p_ known_types;
  print_endline "";;

let prototype2string (retval,funcname,argvs,decorators,code,is_const) =
  let argvs_str =
    String.concat ", " (List.map (fun (t,n)-> (to_string t) ^ " " ^ n) argvs) in
  let const_statement = if is_const then "const" else "" in
  let decorators = String.concat " " decorators in
  Printf.sprintf "=> %s %s(%s)%s; with code of %d bytes %s" (* \033[34m escape sequence... for colored term*)
    (to_string retval) funcname argvs_str const_statement (String.length code) decorators;;
  
let print_classimpl = function
  | ClassImpl(name,funcs,members) ->
    print_endline ("classname => " ^ name ^ ":");
    print_endline "methods:";
    List.iter (Util.compose print_endline prototype2string) funcs;
    let print_member (t,n) =
      Printf.printf "=> %s %s;\n" (to_string t) n
    in
    print_endline "members:";
    List.iter print_member members;;
