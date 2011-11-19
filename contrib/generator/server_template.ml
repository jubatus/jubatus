
exception Not_class_impl

(* used in client *)
let make_class_begin classname =
  "class " ^ classname ^ "{\npublic:\n"
  (* ^ "  " ^ classname ^ "(vector<connection_info> servers, string name, int timeout);\n" *)
  (*:\  servers_(servers), name_(name), timeout_(timeout)*)
  (*^ "  ~" ^ classname ^ "();\n" *);;

(* used in client *)
let make_class_end classname =
  "}; // " ^ classname ;;

let prototype2impl (t,n,argvs,decorators,code,is_const) =
  let argvs_str =
    String.concat ", " (List.map (fun (t,n)-> (Stree.to_string t) ^ " " ^ n) argvs) in
  let const_statement = if is_const then "const" else "" in
  let decorators = String.concat " " decorators in
  
  Printf.sprintf "  %s %s(%s) %s %s \n  %s;\n"
    (Stree.to_string t) n (argvs_str) const_statement decorators code;;

let memberdecl (t,n) =
  Printf.sprintf "  %s %s;\n" (Stree.to_string t) n;;

let make_class = function
  | Stree.ClassImpl(classname, funcs, members) ->
    make_class_begin classname
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "private:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";
(*  | _ -> raise Not_class_impl;; *)

(* output <<< Server_template.constructor name;
      output <<< Server_template.destructor name; *)


(*
let constructor classname =
  classname^"::"^classname^"(vector<connection_info> servers, string name, int timeout)\n  \
      :servers_(servers), name_(name), timeout_(timeout)\
      {\n}";;

let destructor classname = classname^"::~"^classname^"(){\n}";;

let prototype2string _ _ = "prototype2string";;
*)

