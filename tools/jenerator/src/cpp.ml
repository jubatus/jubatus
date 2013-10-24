(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and
 Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License version 2.1 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

(* TODO(unnonouno): split source *)

open Syntax
open Lib

let comment_out_head = "//"

let gen_jubatus_core_include conf file =
  let path =
    if conf.Config.internal then
      "\"jubatus/core/" ^ file ^ "\""
    else
      "<jubatus/core/" ^ file ^ ">"
  in
  "#include " ^ path
;;

let gen_jubatus_include conf file =
  let path =
    if conf.Config.internal then
      "\"../../" ^ file ^ "\""
    else
      "<jubatus/" ^ file ^ ">"
  in
  "#include " ^ path
;;

let gen_datum_include conf server =
  let path =
    if server then
      if conf.Config.internal then
        "\"jubatus/core/fv_converter/datum.hpp\""
      else
        "<jubatus/core/fv_converter/datum.hpp>"
    else
      "<jubatus/client/common/datum.hpp>"
  in
  "#include " ^ path
;;

let gen_jubatus_client_include conf file =
  let path =
    if conf.Config.internal then
      "\"" ^ file ^ "\""
    else
      "<jubatus/client/" ^ file ^ ">"
  in
  "#include " ^ path
;;

let parse_namespace namespace =
  Str.split (Str.regexp "::") namespace
;;

let rec gen_template names server typ args =
  let arg_strs = List.map (gen_type names server) args in
  gen_template_with_strs typ arg_strs

and gen_template_with_strs typ arg_strs =
  let s = String.concat ", " arg_strs in
  let len = String.length s in
  if len = 0 || s.[len - 1] = '>' then
    typ ^ "<" ^ s ^ " >"
  else
    typ ^ "<" ^ s ^ ">"

and gen_type names server = function
  | Object -> raise (Unknown_type "Object is not supported")
  | Bool -> "bool"
  | Int(signed, bytes) -> begin
    match signed, bytes with
    | true, 1 -> "int8_t"
    | true, 2 -> "int16_t"
    | true, 4 -> "int32_t"
    | true, 8 -> "int64_t"
    | false, 1 -> "uint8_t"
    | false, 2 -> "uint16_t"
    | false, 4 -> "uint32_t"
    | false, 8 -> "uint64_t"
    | _ -> raise (Unknown_type (Printf.sprintf "unknonw int type: %b, %d" signed bytes))
  end
  | Float false -> "float"
  | Float true -> "double"
  | Raw -> "std::string"
  | String -> "std::string"
  | Datum ->
    if server then
      "jubatus::core::fv_converter::datum"
    else
      "jubatus::client::common::datum"
  | Struct s ->
    if Hashtbl.mem names s then
      Hashtbl.find names s
    else
      s
  | List typ -> gen_template names server "std::vector" [typ]
  | Map(key, value) -> gen_template names server "std::map" [key; value]
  | Nullable typ ->  raise (Unknown_type "Nullable is not supported")
;;

let gen_argument_type names server = function
  | Raw
  | String
  | Datum
  | Struct _
  | List _
  | Map(_, _) as t ->
    "const " ^ gen_type names server t ^ "&"
  | _ as t ->
    gen_type names server t
;;

let gen_bool_literal = function
  | true -> "true"
  | false -> "false"
;;

let gen_string_literal s =
  "\"" ^ String.escaped s ^ "\""
;;

let gen_args args =
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_arg_def names server f =
  (gen_argument_type names server f.field_type) ^ " " ^ f.field_name
;;

let gen_field_def names server f =
  (gen_type names server f.field_type) ^ " " ^ f.field_name
;;

let gen_function_args_def names server args =
  let vars = List.map (gen_arg_def names server) args in
  gen_args vars
;;

let gen_reference_arg_def names f =
  let typ = gen_type names true f.field_type in
  let ref_type =
    match f.field_type with
    | Bool | Int _ | Float _ -> typ
    | _ -> "const " ^ typ ^ "&"
  in
  ref_type ^ " " ^ f.field_name
;;

let gen_function_reference_args_def names args =
  let vars = List.map (gen_reference_arg_def names) args in
  gen_args vars
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args ^ ";"
;;

let gen_ret_type names server  = function
  | None -> "void"
  | Some typ -> gen_type names server typ
;;

let make_guard_name filename =
  let name = Filename.basename filename in
  let upper = String.uppercase name in
  Str.global_replace (Str.regexp "[\\./]") "_" upper ^ "_"
;;

let make_header_impl for_template conf source filename content =
  let guard = conf.Config.include_guard ^ make_guard_name filename in
  make_source_impl for_template conf source filename (concat_blocks [
    [
      (0, "#ifndef " ^ guard);
      (0, "#define " ^ guard);
    ];
    content;
    [
      (0, "#endif  // " ^ guard);
    ]
  ]) comment_out_head
;;

let make_header = make_header_impl false
;;

let make_template_header = make_header_impl true
;;


let rec make_namespace ns content =
  match ns with
  | [] ->
    List.concat [
      [ (0, "") ];
      content;
      [ (0, "") ];
    ]
  | n::ns ->
    List.concat [
      [ (0, "namespace " ^ n ^ " {") ];
      make_namespace ns content;
      [ (0, "}  // namespace " ^ n) ];
    ]
;;

let gen_client_method names server m =
  let name = m.method_name in
  let args_def = gen_function_args_def names server m.method_arguments in
  let args = gen_string_literal name
    :: "name_"
    :: List.map (fun f -> f.field_name) m.method_arguments in
  let ret_type = gen_ret_type names server m.method_return_type in

  let call =
    match m.method_return_type with
    | None ->
      [ (0, gen_call "c_.call" args) ]
    | Some typ ->
      [
        (0, gen_call "msgpack::rpc::future f = c_.call" args);
        (0, "return " ^ (gen_template names server "f.get" [typ]) ^ "();");
      ] in
  List.concat [
    [
      (0, Printf.sprintf "%s %s%s {" ret_type name args_def);
    ];
    indent_lines 1 call;
    [
      (0, "}");
    ]
  ]
;;

let gen_client names server s =
  let methods = List.map (gen_client_method names server) s.service_methods in
  let constructor = [
    (0, s.service_name ^ "(const std::string& host, uint64_t port, const std::string& name, unsigned int timeout_sec)");
    (2,     ": client(host, port, name, timeout_sec) {");
    (0, "}");
  ] in
  let content = concat_blocks (constructor::methods) in

  List.concat [
    [
      (0, "class " ^ s.service_name ^ " : public jubatus::client::common::client {");
      (0, " public:");
    ];
    indent_lines 1 content;
    [
      (0, "};")
    ]
  ]
;;

let gen_message_field names server f =
  (0, gen_field_def names server f ^ ";")
;;

let gen_message names server m =
  let fields = List.map (gen_message_field names server) m.message_fields in
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let msgpack_define = gen_call "MSGPACK_DEFINE" field_names in
  let default_constructor = [
    (0, m.message_name ^ "() {");
    (0, "}");
  ] in
  let args_def = gen_function_args_def names server m.message_fields in
  let assigns = List.map (fun f -> f.field_name ^ "(" ^ f.field_name ^ ")") m.message_fields in
  let explicit =
    if List.length m.message_fields = 1 then
      "explicit "
    else
      "" in
  let constructor = [
    (0, explicit ^ m.message_name ^ args_def);
    (1,   ": " ^ String.concat ", " assigns ^ " {");
    (0, "}");
  ] in
  List.concat [
    [
      (0, "struct " ^ m.message_name ^ " {");
      (0, " public:");
      (1,   msgpack_define);
    ];
    indent_lines 1 fields;
    indent_lines 1 default_constructor;
    indent_lines 1 constructor;
    [ (0, "};")]
  ]
;;


let gen_typedef names server = function
  | Message m ->
    begin match m.message_raw with
    | Some raw when Hashtbl.mem names m.message_name -> []
    | _ -> gen_message names server m
    end
  | _ ->
    []
;;

let collect_names prog =
  let map = Hashtbl.create 10 in
  List.iter (function
  | Message m ->
    begin match m.message_raw with
    | Some raw -> Hashtbl.add map m.message_name raw
    | None -> ()
    end
  | _ -> ()
  ) prog;
  map
;;

let gen_client_file conf names server source services =
  let base = File_util.take_base source in
  (* TODO(unnonouno): smarter way? *)
  let filename = base ^ "_client.hpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["client"] in
  let clients = List.map (gen_client names server) services in

  let content = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <jubatus/client/common/client.hpp>");
      (0, gen_datum_include conf server);
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (concat_blocks clients)
  ] in

  make_header conf source filename content
;;

let make_includes idl =
  List.concat
    (List.map (function
    | Include s -> [ (0, "#include " ^ gen_string_literal s) ]
    | _ -> []
     ) idl)
;;

let gen_type_file conf names server source idl =
  let base = File_util.take_base source in
  let name = base ^ "_types.hpp" in
  let namespace = parse_namespace conf.Config.namespace in
  let types = List.map (gen_typedef names server) idl in
  let includes = [
    (0, "#include <stdint.h>");
    (0, "");
    (0, "#include <map>");
    (0, "#include <string>");
    (0, "#include <vector>");
    (0, "#include <utility>");
    (0, "");
    (0, gen_datum_include conf server);
    (0, "#include <msgpack.hpp>");
  ] in

  let custom_includes = make_includes idl in

  let content = concat_blocks [
    includes;
    custom_includes;
    make_namespace namespace (concat_blocks types);
  ] in

  make_header conf source name content
;;

let get_func_type names m =
  let ret_type = gen_ret_type names true m.method_return_type in
  let ts = List.map (fun f -> f.field_type) m.method_arguments in
  (* Insert String to the first argument, which represents the cluster name *)
  let arg_types = List.map (fun t -> gen_type names true t) (String::ts) in
  let args = gen_args arg_types in
  ret_type ^ args
;;

let gen_bind s m =
  let num_args = List.length m.method_arguments in
  let func = "&" ^ s.service_name ^ "_impl::" ^ m.method_name in
  let this = "this" in
  (* Ignore the first argument as it is cluster name *)
  let nums = Array.init num_args (fun n -> Printf.sprintf "pfi::lang::_%d" (n + 2)) in
  let args = func::this::(Array.to_list nums) in
  "pfi::lang::bind" ^ gen_args args
;;

let gen_server_method names s m =
  let func_type = get_func_type names m in
  let method_name_str = gen_string_literal m.method_name in
  let bind = gen_bind s m in
  let line = Printf.sprintf "rpc_server::add<%s>(%s, %s);"
    func_type method_name_str bind in
  (0, line)
;;

let gen_aggregator names ret_type aggregator =
  match ret_type, aggregator with
  | Bool, All_and ->
    "jubatus::server::framework::all_and"
  | Bool, All_or ->
    "jubatus::server::framework::all_or"
  | List t, Concat ->
    gen_template names true "jubatus::server::framework::concat" [t]
  | Map (k, v), Merge ->
    gen_template names true "jubatus::server::framework::merge" [k; v]
  | _, Pass ->
    gen_template names true "jubatus::server::framework::pass" [ret_type]
  | _, _ ->
    (* TODO(unnonouno): Are other combinations really illegal?*)
    let msg = Printf.sprintf
      "invalid combination of return type and aggretator type: %s and %s"
      (gen_type names true ret_type) (aggtype_to_string aggregator) in
    raise (Invalid_argument msg)
;;

let gen_aggregator_function names ret_type aggregator =
  let agg = gen_aggregator names ret_type aggregator in
  let r = gen_type names true ret_type in
  (* TODO(unnonouno): Too complicated. Make it simple! *)
  let func = Printf.sprintf
    "pfi::lang::function<%s(%s, %s)>" r r r in
  func ^ "(&" ^ agg ^ ")"
;;

let gen_proxy_register names m ret_type =
  let arg_types = List.map (fun f -> f.field_type) m.method_arguments in
  let method_name_str = gen_string_literal m.method_name in
  let routing, _, agg = get_decorator m in
  match routing with
  | Random ->
    let func = gen_template names true "k.register_async_random" (ret_type::arg_types) in
    let call = gen_call func [method_name_str] in
    [ (0, call) ]

  | Cht i ->
    (* When a user use CHT, the first arguemnt is the hashing key *)
    let args = List.tl arg_types in
    let arg_strs = List.map (gen_type names true) (ret_type::args) in
    (* TODO(unnonouno): Is this number really required to be a template argument? *)
    let num = string_of_int i in
    let func = gen_template_with_strs "k.register_async_cht" (num::arg_strs) in
    let call = gen_call func [method_name_str; gen_aggregator_function names ret_type agg] in
    [ (0, call) ]

  | Broadcast ->
    let func = gen_template names true "k.register_async_broadcast" (ret_type::arg_types) in
    let call = gen_call func [method_name_str; gen_aggregator_function names ret_type agg] in
    [ (0, call) ]

  | Internal -> (* no code generated in proxy *)
    []
;;

let gen_proxy_method names m =
  match m.method_return_type with
  | Some ret_type ->
    gen_proxy_register names m ret_type
  | None ->
    (* TODO(unnonouno): How to treat funcitons that return no values? *)
    []
;;

let gen_proxy names s =
  List.map (gen_proxy_method names) s.service_methods
;;

let gen_proxy_file conf names source services =
  let base = File_util.take_base source in
  let filename = base ^ "_proxy.cpp" in

  let name_str = gen_string_literal base in
  let servers = List.concat (List.map (gen_proxy names) services) in

  let namespace = parse_namespace conf.Config.namespace in
  let func = String.concat "::" namespace ^ "::run_proxy" in

  let s = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <utility>");
      (0, "");
      (0, "#include <glog/logging.h>");
      (0, "");
      (0, gen_jubatus_core_include conf "common/exception.hpp");
      (0, gen_jubatus_include conf "server/framework/aggregators.hpp");
      (0, gen_jubatus_include conf "server/framework/proxy.hpp");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (
      List.concat [
        [
          (0, "int run_proxy(int argc, char* argv[]) {");
          (1,   "try {");
          (2,     "jubatus::server::framework::proxy k(");
          (4,         "jubatus::server::framework::proxy_argv(argc, argv, " ^ name_str ^ "));");
        ];
        indent_lines 2 (List.concat servers);
        [
          (2,     "return k.run();");
          (1,   "} catch (const jubatus::core::common::exception::jubatus_exception& e) {");
          (2,     "LOG(FATAL) << e.diagnostic_information(true);");
          (2,     "return -1;");
          (1,   "}");
          (0, "}")
        ]
      ]
    );
    [
      (0, "int main(int argc, char* argv[]) {");
      (1,   func ^ "(argc, argv);");
      (0, "}")
    ]
  ] in
  make_source conf source filename s comment_out_head
;;

let gen_impl_method names m =
  let name = m.method_name in
  let args_def = gen_function_args_def names true m.method_arguments in
  let args = List.map (fun f -> f.field_name) m.method_arguments in
  let ret_type = gen_ret_type names true m.method_return_type in

  let _, request, _ = get_decorator m in
  let lock_type =
    match request with
    | Update -> "JWLOCK_"
    | Analysis -> "JRLOCK_"
    | Nolock -> "NOLOCK_" in
  let lock = gen_call lock_type ["p_"] in
  let call = gen_call ("get_p()->" ^ name) args in
  let call =
    match m.method_return_type with
    | None ->
      call
    | Some typ ->
      "return " ^ call
  in

  [
    (0, Printf.sprintf "%s %s%s {" ret_type name args_def);
    (1,   lock);
    (1,   call);
    (0, "}");
  ]
;;

let is_cht_method m =
  let routing, _, _ = get_decorator m in
  match routing with
  | Cht _ -> true
  | _ -> false
;;

let include_cht_method s =
  List.exists is_cht_method s.service_methods
;;

let gen_impl names s =
  let register_methods = List.map (gen_server_method names s) s.service_methods in
  let methods = List.map (gen_impl_method names) s.service_methods in
  let name = s.service_name in
  let impl_name = name ^ "_impl" in
  let serv_name = name ^ "_serv" in
  let use_cht = include_cht_method s in
  concat_blocks [
    [
      (0, "class " ^ impl_name ^ " : public jubatus::server::common::mprpc::rpc_server {");
      (0, " public:");
      (1,   "explicit " ^ impl_name ^ "(const jubatus::server::framework::server_argv& a):");
      (2,     "rpc_server(a.timeout),");
      (2,     "p_(new jubatus::server::framework::server_helper<" ^ serv_name ^ ">(a, " ^ gen_bool_literal use_cht ^ ")) {");
    ];
    indent_lines 2 register_methods;
    [
      (* TODO(unno): use base class? *)
      (2,   "rpc_server::add<std::string(std::string)>(\"get_config\", pfi::lang::bind(&"
        ^ impl_name ^ "::get_config, this));");
      (2,   "rpc_server::add<bool(std::string, std::string)>(\"save\", pfi::lang::bind(&"
        ^ impl_name ^ "::save, this, pfi::lang::_2));");
      (2,   "rpc_server::add<bool(std::string, std::string)>(\"load\", pfi::lang::bind(&"
        ^ impl_name ^ "::load, this, pfi::lang::_2));");
      (2,    "rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(std::string)>(\"get_status\", pfi::lang::bind(&"
        ^ impl_name ^ "::get_status, this));");
      (1,   "}");
    ];
    indent_lines 1 (concat_blocks methods);
    [
      (* TODO(unno): use base class? *)
      (1,   "std::string get_config() {");
      (2,     "JRLOCK_(p_);");
      (2,     "return get_p()->get_config();");
      (1,   "}");
    ];
    [
      (1,   "bool save(const std::string& id) {");
      (2,     "JWLOCK_(p_);");
      (2,     "return get_p()->save(id);");
      (1,   "}");
    ];
    [
      (1,   "bool load(const std::string& id) {");
      (2,     "JWLOCK_(p_);");
      (2,     "return get_p()->load(id);");
      (1,   "}");
    ];
    [
      (1,   "std::map<std::string, std::map<std::string, std::string> > get_status() {");
      (2,     "JRLOCK_(p_);");
      (2,     "return p_->get_status();");
      (1,   "}");
    ];
    [
      (1,   "int run() { return p_->start(*this); }");
      (1,   "pfi::lang::shared_ptr<" ^ serv_name ^ "> get_p() { return p_->server(); }");
      (0, "");
      (0, " private:");
      (1,   "pfi::lang::shared_ptr<jubatus::server::framework::server_helper<" ^ serv_name ^ "> > p_;");
      (0, "};")
    ]
  ]
;;

let gen_impl_file conf names source services =
  let base = File_util.take_base source in
  let filename = base ^ "_impl.cpp" in
  let name_str = gen_string_literal base in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let namespace_str = String.concat "::" namespace in
  let impls = List.map (gen_impl names) services in
  let s = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <utility>");
      (0, "#include <pficommon/lang/shared_ptr.h>");
      (0, "");
      (0, gen_jubatus_include conf "server/framework.hpp");
      (0, "#include \"" ^ base ^ "_serv.hpp\"");
    ];
    make_namespace namespace (List.concat impls);
    [
      (0, "int main(int argc, char* argv[]) {");
      (1,   "return");
      (* TODO(unnonouno): does not work when service name is not equal to a source file*)
      (2,     "jubatus::server::framework::run_server<" ^ namespace_str ^ "::" ^ base ^ "_impl>");
      (3,       "(argc, argv, " ^ name_str ^ ");");
      (0, "}")
    ]
  ] in
  make_source conf source filename s comment_out_head
;;

let gen_const m =
  let _, rw, _ = get_decorator m in
  match rw with
  | Update -> ""
  | Analysis -> " const"
  | Nolock -> " /* nolock!! */"
;;

let gen_server_template_header_method names m =
  let name = m.method_name in
  let args_def = gen_function_reference_args_def names m.method_arguments in
  let ret_type = gen_ret_type names true m.method_return_type in
  let const = gen_const m in
  [ (0, Printf.sprintf "%s %s%s%s;" ret_type name args_def const) ]
;;

(* TODO(unnonouno): These special methods are going to be removed from IDL *)
let filter_methods methods =
  List.filter (fun m ->
    not (m.method_name = "save"
         || m.method_name = "load"
         || m.method_name = "get_status")
  ) methods
;;

let gen_server_template_header names s =
  let methods = List.map (gen_server_template_header_method names) s.service_methods in
  let name = s.service_name in
  let serv_name = name ^ "_serv" in
  List.concat [
    [
      (0, "class " ^ serv_name ^ " : public jubatus::server::framework::server_base {  // do not change");
      (0, " public:");
      (1,   serv_name ^ "(");
      (2,     "const jubatus::server::framework::server_argv& a,");
      (2,     "const pfi::lang::shared_ptr<jubatus::server::common::lock_service>& zk);  // do not change");
      (1,   "virtual ~" ^ serv_name ^ "();  // do not change");
      (0,   "");
      (1,   "virtual jubatus::server::framework::mixer::mixer* get_mixer() const;");
      (1,   "pfi::lang::shared_ptr<jubatus::core::framework::mixable_holder> get_mixable_holder() const;");
      (1,   "std::string get_config() const;");
      (1,   "void get_status(status_t& status) const;");
      (1,   "void set_config(const std::string& config);");
      (0,   "");
    ];
    indent_lines 1 (List.concat methods);
    [
      (0, "");
      (0, " private:");
      (1,   "// add user data here like: pfi::lang::shared_ptr<some_type> some_;");
      (0, "};")
    ]
  ]
;;

let gen_server_template_header_file conf names source services =
  let base = File_util.take_base source in
  let filename = base ^ "_serv.tmpl.hpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let servers = List.map (gen_server_template_header names) services in

  let content = concat_blocks [
    [
      (0, "#include <string>");
      (0, gen_jubatus_include conf "server/framework.hpp");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (concat_blocks servers)
  ] in

  make_template_header conf source filename content
;;

let gen_server_template_source_method names s m =
  let serv_name = s.service_name ^ "_serv" in

  let name = m.method_name in
  let args_def = gen_function_reference_args_def names m.method_arguments in
  let ret_type = gen_ret_type names true m.method_return_type in
  let const = gen_const m in
  [
    (0, Printf.sprintf "%s %s::%s%s%s {" ret_type serv_name name args_def const);
    (0, "}");
  ]
;;

let gen_server_template_source names s =
  let methods = List.map (gen_server_template_source_method names s) s.service_methods in
  let name = s.service_name in
  let serv_name = name ^ "_serv" in
  concat_blocks [
    [
      (0, serv_name ^ "::" ^ serv_name ^ "(");
      (1,   "const jubatus::server::framework::server_argv& a,");
      (1,   "const pfi::lang::shared_ptr<jubatus::server::common::lock_service>& zk)");
      (2,     ": jubatus::server::framework::server_base(a) {");
      (1,   "// somemixable* mi = new somemixable;");
      (1,   "// somemixable_.set_model(mi);");
      (1,   "// get_mixable_holder()->register_mixable(mi);");
      (0, "}");
      (0, "");
      (0, serv_name ^ "::~" ^ serv_name ^ "() {");
      (0, "}");
      (0, "");
      (0, "jubatus::server::framework::mixer::mixer* " ^ serv_name ^ "::get_mixer() const {");
      (0, "}");
      (0, "");
      (0, "pfi::lang::shared_ptr<jubatus::core::framework::mixable_holder> "
        ^ serv_name ^ "::get_mixable_holder() const {");
      (0, "}");
      (0, "");
      (0, "std::string " ^ serv_name ^ "::get_config() const {");
      (0, "}");
      (0, "");
      (0, "void " ^ serv_name ^ "::get_status(status_t& status) const {");
      (0, "}");
      (0, "");
      (0, "void " ^ serv_name ^ "::set_config(const std::string& config) {");
      (0, "}");
      (0, "");
    ];
    concat_blocks methods;
  ]
;;

let gen_server_template_source_file conf names source services =
  let base = File_util.take_base source in
  let serv = base ^ "_serv" in
  let serv_hpp = base ^ "_serv.hpp" in
  let filename = serv ^ ".tmpl.cpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let servers = List.map (gen_server_template_source names) services in

  let content = concat_blocks [
    [
      (0, "#include \"" ^ serv_hpp ^ "\"");
    ];
    make_namespace namespace (concat_blocks servers);
  ] in

  make_template_source conf source filename content comment_out_head
;;

let generate_server conf source idl =
  let services = get_services idl in
  let names = collect_names idl in

  gen_type_file conf names true source idl;
  gen_client_file conf names true source services;
  gen_proxy_file conf names source services;
  gen_impl_file conf names source services;
  if conf.Config.default_template then begin
    gen_server_template_header_file conf names source services;
    gen_server_template_source_file conf names source services;
  end
;;

let generate_client conf source idl =
  let services = get_services idl in
  (* In client codes, we need not to use %include *)
  let empty_names = Hashtbl.create 0 in
  (* Remove include lines that are only for servers *)
  let idl = List.filter (function Include _ -> false | _ -> true) idl in

  gen_type_file conf empty_names false source idl;
  gen_client_file conf empty_names false source services;
;;
