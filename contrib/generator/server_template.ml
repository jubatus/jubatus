
exception Not_class_impl

let make_class_begin classname =
  Printf.sprintf "class %s_impl : public rpc_server<%s_impl> \n{\n"
    classname classname;;

let make_class_end classname =
  Printf.sprintf "  shared_ptr<%s> p_;\n};\n" classname;;

let prototype2impl (t,n,argvs,decorators,is_const) =
  let rec argvs_str str i = function
    | [] -> str;
    | hd::tl -> argvs_str (str ^ (Stree.to_string hd) ^ " arg" ^ (string_of_int i)) (i+1) tl
  in
  let const_statement = if is_const then "const" else "" in
  let decorators = String.concat " " decorators in
  let argvs_str2 n =
    let rec make_list l = function
      | 0 -> l
      | i -> make_list ((n-i)::l) (i-1)
    in
    String.concat ", " (List.map (fun i-> "arg" ^ string_of_int i) (make_list [] n))
  in
  let make_return_statement = function
    | Stree.Void -> "";
    | _ -> "return "
  in
  Printf.sprintf "  %s %s(%s)%s %s\n  { %sp_->%s(%s); };\n"
    (Stree.to_string t) n (argvs_str "" 0 argvs) const_statement decorators
    (make_return_statement t) n (argvs_str2 (List.length argvs))
;;

let memberdecl (t,n) =
  Printf.sprintf "  %s %s;\n" (Stree.to_string t) n;;

let make_class = function
  | Stree.ClassDef(classname, funcs, members) ->
    make_class_begin classname
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "\nprivate:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";;

let make_main _ =
  "int main(int args, char** argv){\n"
  ^ Printf.sprintf "  run_server<%s>(args, argv);\n" "FIXME"
  ^ "}";;
