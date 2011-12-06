
let make_file_begin name =
  "#include \"keeper.hpp\"\n"
  ^Printf.sprintf "#include \"%s_types.hpp\"\n" name
;;

let make_file_end _ = "";;

let rec to_string = function
  | Stree.Void -> "void";
  | Stree.Int  -> "int";
  | Stree.Char -> "char";
  | Stree.Double -> "double";
  | Stree.Other(ud, is_ref) ->
    let rec ud_type2string = function
      | Stree.Class(s) -> s;
      | Stree.Struct(s) -> s;
      | Stree.Template(n, l) when (n = "tuple" or n = "std::tuple") && ((List.length l) = 2) ->
	let list = List.map to_string l in
	"std::pair<" ^ (String.concat ", " list) ^ "> " ;
      | Stree.Template(n, l) ->
	let list = List.map to_string l in
	n ^ "<" ^ (String.concat ", " list) ^ "> " ;
(*      | Namespace(ns, ud) -> ns ^ "::" ^ (ud_type2string ud) *)
    in
    ud_type2string ud;
  | Stree.Destructor -> "";
  | Stree.Constructor -> "";;


let generate_class_keeper clazz =
  let Stree.ClassDef(_, prototypes, _) = clazz in
  let prototype2register p =
    let const2type = function
      | true -> "analysis";
      | false -> "update";
    in
    let decorators2str decorators =
      let decorator2str str =
	String.sub str 3 ((String.length str) - 3)
      in
      List.fold_left (fun  _ d -> decorator2str d) "" decorators
    in
    let make_rettype is_const rettype =
      if is_const then (Stree.to_string rettype) ^ ", "
      else ""
    in
    let (rettype, name, argtypes, decorators, is_const) = p in
    Printf.sprintf "  k.register_%s_%s<%s%s>(\"%s\");"
      (decorators2str decorators) (const2type is_const) (make_rettype is_const rettype)
      (String.concat ", " (List.map to_string (List.tl argtypes))) name
  in
  let filter (_,_,_,decorators,_) = not (List.mem "//@fail_in_keeper" decorators) in
  String.concat "\n" (List.map prototype2register (List.filter filter prototypes));;

(*  | _ -> raise Stree.Unkown_directive;; *)
let make_main classdefs =
  "int main(int args, char** argv){\n"
  ^ "  keeper k(jubatus::keeper_argv(args, argv));\n"
  ^ String.concat "\n" (List.map generate_class_keeper classdefs)
  ^ "\n  k.start();"
  ^ "\n}";;

