
(* used in client *)
let make_class_begin modname =
  "class " ^ modname ^ "{\npublic:\n"
  ^ "  " ^ modname ^ "(vector<connection_info> servers, string name, int timeout);\n" (*:\  servers_(servers), name_(name), timeout_(timeout)*)
  ^ "  ~" ^ modname ^ "();\n";;

(* used in client *)
let make_class_end modname =
  "private:\n"
  ^ "  "^modname^"_client make_client() {\n"
  ^ "    connection_info conn = servers_[rng_(servers_.size())];\n"
  ^ "    return "^modname^"_client(conn.first, conn.second, timeout_);\n  };\n"
  ^ "  template <typename T> inline T return_or_throw(const result<T>& res) {\n"
  ^ "    if (!res.success) { throw std::runtime_error(res.error); }\n"
  ^ "    else { return res.retval; }\n  \
};\n"
  ^ "  inline void check_throw(const result<int>& res) {\
  if (!res.success) { \
    throw std::runtime_error(res.error); \
  } \
};\n"
  ^ "  vector<connection_info> servers_;\n"
  ^ "  string name_;\n"
  ^ "  int timeout_;\n"
  ^ "  pfi::math::random::mtrand rng_;\n"
  ^ "};\n";;

let constructor classname =
  classname^"::"^classname^"(vector<connection_info> servers, string name, int timeout)\n  \
      :servers_(servers), name_(name), timeout_(timeout)\
      {\n}";;

let destructor classname = classname^"::~"^classname^"(){\n}";;


(* FIXME: make this for keeper impl (currently for client) *)
let generate_directive2 retval methname argv = function
  | "@broadcast" ->
    (Client_template.return_statement retval) ^ "(make_client().call_" ^ methname ^ "("
    ^ (String.concat ", " argv) ^ "));";
  | "@random" ->
    (Client_template.return_statement retval) ^ "(make_client().call_" ^ methname ^ "("
    ^ (String.concat ", " argv) ^ "));";
(*  | "@cht" ->  "//cht!"; *)
  | _ -> raise Stree.Unkown_directive;;


let prototype2string classname (t,name,list,dir) =
  
  "prototype2string";;


let prototype2impl modname (t,name,list,decs) =
  let arr = Array.mapi (fun i argv -> (Stree.to_string argv) ^ " arg" ^ string_of_int i) (Array.of_list list) in
  let argv = "name_" :: (Array.to_list (Array.mapi (fun i _ -> "arg" ^ string_of_int i) (Array.of_list list))) in
  "// " ^ (String.concat ", " decs) ^ "\n"
  ^ (Stree.to_string t) ^ " " ^ modname ^ "::" ^ name ^ "("
  ^ (String.concat ", " (Array.to_list arr)) ^ "){\n"
  ^ (String.concat "\n" (List.map (generate_directive2 t name argv) decs)) ^ "\n"
  ^ "}" ;;
