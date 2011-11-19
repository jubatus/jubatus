
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


let prototype2string _ _ = "prototype2string";;

let prototype2impl _ _ = "prototype2impl";;
