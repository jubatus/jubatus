
exception Not_class_impl

let make_class_begin classname =
  Printf.sprintf "class %s_impl_ : public %s<%s_impl_> \n{\n"
    classname classname classname;;

let make_class_end classname =
  Printf.sprintf "  pfi::lang::shared_ptr<%s_serv> p_;\n};\n" classname;;

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
    ^ "public:\n"
    ^ Printf.sprintf "  %s_impl_(int args, char** argv)\n" classname
    ^ Printf.sprintf "    : p_(new %s_serv(args, argv)){};\n" classname
    ^ (String.concat "\n" (List.map prototype2impl funcs))
    ^ "\n  int start(){return -1;}; // FIXME \n"
    ^ "\nprivate:\n"
    ^ (String.concat "" (List.map memberdecl members))
    ^ make_class_end classname
    ^ "";;

let make_main namespaces classdef =
  let namespace = String.concat "::" namespaces in
  let Stree.ClassDef(classname, _, _) = classdef in
  "int main(int args, char** argv){\n"
  ^ Printf.sprintf "  return jubatus::run_server<%s::%s_impl_>(args, argv);\n"
    namespace classname
  ^ "}";;
