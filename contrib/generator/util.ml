
let output_endline out_channel str =
  output_string out_channel str;
  output_char out_channel '\n';;

let (<<<) = output_endline;;

let make_ns_begin c namespace =
  "namespace " ^ namespace ^ " { namespace " ^ c ^ " {";;

let make_ns_end c namespace =
  "}} // namespace " ^ namespace ^ "::" ^ c ;;

let make_header_header =
  "#pragma once\
";;

let include_b files =
  let f file = "#include <" ^ file ^ ">" in
  String.concat "\n" (List.map f files);;

let include_dq files =
  let f file = "#include \"" ^ file ^ "\"" in
  String.concat "\n" (List.map f files);;

let make_file_begin c namespace =
  "#pragma once\n"
  ^ "#include <vector>\n"
  ^ "#include <string>\n"
  ^ "using std::vector;\n"
  ^ "using std::string;\n" ^ make_ns_begin c namespace;;

let make_file_end = make_ns_end;;


let compose f g x = f (g x);;

