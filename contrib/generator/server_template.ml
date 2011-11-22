
exception Not_class_impl

(* used in client *)
let make_class_begin classname =
  "class " ^ classname ^ "{\npublic:\n"

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

(* FIXME: filter bad functions included in 'funcs' that pretend to be constructor *)
let make_class = function
  | Stree.ClassImpl(classname, funcs, members) ->
    make_class_begin classname
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "private:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";;
(*
let prototype2string _ _ = "prototype2string";;
*)


let make_main() =
  "int main(int args, char** argv){\n"
  ^ "  " 
  ^ "}";;
