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
      "\"../" ^ file ^ "\""
    else
      "<jubatus/" ^ file ^ ">"
  in
  "#include " ^ path
;;

let parse_namespace namespace =
  Str.split (Str.regexp "::") namespace
;;

let rec gen_template typ args =
  let arg_strs = List.map gen_type args in
  gen_template_with_strs typ arg_strs

and gen_template_with_strs typ arg_strs =
  let s = String.concat ", " arg_strs in
  let len = String.length s in
  if len = 0 or s.[len - 1] = '>' then
    typ ^ "<" ^ s ^ " >"
  else
    typ ^ "<" ^ s ^ ">"

and gen_type = function
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
  | Struct s -> s
  | List typ -> gen_template "std::vector" [typ]
  | Map(key, value) -> gen_template "std::map" [key; value]
  | Tuple [t1; t2] -> gen_template "std::pair" [t1; t2]
  | Tuple typs ->  raise (Unknown_type "Tuple is not supported")
  | Nullable typ ->  raise (Unknown_type "Nullable is not supported")
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

let gen_arg_def f =
  (gen_type f.field_type) ^ " " ^ f.field_name
;;

let gen_function_args_def args =
  let vars = List.map gen_arg_def args in
  gen_args vars
;;

let gen_reference_arg_def f =
  let typ = gen_type f.field_type in
  let ref_type =
    match f.field_type with
    | Bool | Int _ | Float _ -> typ
    | _ -> "const " ^ typ ^ "&"
  in
  ref_type ^ " " ^ f.field_name
;;

let gen_function_reference_args_def args =
  let vars = List.map gen_reference_arg_def args in
  gen_args vars
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args ^ ";"
;;

let gen_ret_type = function
  | None -> "void"
  | Some typ -> gen_type typ
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

let gen_client_method m =
  let name = m.method_name in
  let args_def = gen_function_args_def m.method_arguments in
  let args = gen_string_literal name
    :: List.map (fun f -> f.field_name) m.method_arguments in
  let ret_type = gen_ret_type m.method_return_type in
  
  let call =
    match m.method_return_type with
    | None ->
      [ (0, gen_call "c_.call" args) ]
    | Some typ ->
      [
        (0, gen_call "msgpack::rpc::future f = c_.call" args);
        (0, "return " ^ (gen_template "f.get" [typ]) ^ "();");
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

let gen_client s =
  let methods = List.map gen_client_method s.service_methods in
  let constructor = [
    (0, s.service_name ^ "(const std::string& host, uint64_t port, double timeout_sec)");
    (2,     ": c_(host, port) {");
    (1,   "c_.set_timeout(timeout_sec);");
    (0, "}");
  ] in
  let content = concat_blocks (constructor::methods) in

  List.concat [
    [
      (0, "class " ^ s.service_name ^ " {");
      (0, " public:");
    ];
    indent_lines 1 content;
    [
      (0, "");
      (1, "msgpack::rpc::client& get_client() {");
      (2, "return c_;");
      (1, "}");
      (0, "");
      (0, " private:");
      (1,   "msgpack::rpc::client c_;");
      (0, "};")
    ]
  ]
;;

let gen_message_field f =
  (0, gen_arg_def f ^ ";")
;;

let gen_message m =
  let fields = List.map gen_message_field m.message_fields in
  let field_names = List.map (fun f -> f.field_name) m.message_fields in
  let msgpack_define = gen_call "MSGPACK_DEFINE" field_names in
  List.concat [
    [
      (0, "struct " ^ m.message_name ^ " {");
      (0, " public:");
      (1,   msgpack_define);
    ];
    indent_lines 1 fields;
    [ (0, "};")]
  ]
;;

  
let gen_typedef = function
  | Typedef(name, typ) ->
    [ (0, "typedef " ^ gen_type typ ^ " " ^ name ^ ";") ]
  | Message m ->
    gen_message m
  | _ ->
    []
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  (* TODO(unnonouno): smarter way? *)
  let filename = base ^ "_client.hpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["client"] in
  let clients = List.map gen_client services in

  let content = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <utility>");
      (0, "#include <jubatus/msgpack/rpc/client.h>");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (concat_blocks clients)
  ] in
  
  make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = base ^ "_types.hpp" in
  let namespace = parse_namespace conf.Config.namespace in
  let types = List.map gen_typedef idl in
  let includes = [
    (0, "#include <stdint.h>");
    (0, "");
    (0, "#include <map>");
    (0, "#include <string>");
    (0, "#include <vector>");
    (0, "#include <utility>");
    (0, "");
    (0, "#include <msgpack.hpp>");
  ] in

  let content = concat_blocks [
    includes;
    make_namespace namespace (concat_blocks types);
  ] in

  make_header conf source name content
;;

let get_func_type m =
  let ret_type = gen_ret_type m.method_return_type in
  let arg_types = List.map (fun f -> gen_type f.field_type) m.method_arguments in
  let args = gen_args arg_types in
  ret_type ^ args
;;

let gen_bind m =
  let num_args = List.length m.method_arguments in
  let func = "&Impl::" ^ m.method_name in
  let this = "impl" in
  let nums = Array.init num_args (fun n -> Printf.sprintf "pfi::lang::_%d" (n + 1)) in
  let args = func::this::(Array.to_list nums) in
  "pfi::lang::bind" ^ gen_args args
;;

let gen_server_method m =
  let func_type = get_func_type m in
  let method_name_str = gen_string_literal m.method_name in
  let bind = gen_bind m in
  let line = Printf.sprintf "rpc_server::add<%s>(%s, %s);"
    func_type method_name_str bind in
  (0, line)
;;

let gen_server s =
  let methods = List.map gen_server_method s.service_methods in
  List.concat [
    [
      (0, "template <class Impl>");
      (0, "class " ^ s.service_name ^ " : public jubatus::server::common::mprpc::rpc_server {");
      (0, " public:");
      (1,   "explicit " ^ s.service_name ^ "(double timeout_sec) : rpc_server(timeout_sec) {");
      (2,     "Impl* impl = static_cast<Impl*>(this);");
    ];
    indent_lines 2 methods;
    [
      (1,   "}");
      (0, "};")
    ]
  ]
;;

let gen_server_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_server.hpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let servers = List.map gen_server services in

  let content = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <utility>");
      (0, "#include <pficommon/lang/bind.h>");
      (0, "");
      (0, gen_jubatus_include conf "common/mprpc/rpc_server.hpp");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (concat_blocks servers)
  ] in
  
  make_header conf source filename content
;;

let gen_aggregator ret_type aggregator =
  match ret_type, aggregator with
  | Bool, All_and ->
    "jubatus::server::framework::all_and"
  | Bool, All_or ->
    "jubatus::server::framework::all_or"
  | List t, Concat ->
    gen_template "jubatus::server::framework::concat" [t]
  | Map (k, v), Merge ->
    gen_template "jubatus::server::framework::merge" [k; v]
  | _, Pass ->
    gen_template "jubatus::server::framework::pass" [ret_type]
  | _, _ ->
    (* TODO(unnonouno): Are other combinations really illegal?*)
    let msg = Printf.sprintf
      "invalid combination of return type and aggretator type: %s and %s"
      (gen_type ret_type) (aggtype_to_string aggregator) in
    raise (Invalid_argument msg)
;;

let gen_aggregator_function ret_type aggregator =
  let agg = gen_aggregator ret_type aggregator in
  let r = gen_type ret_type in
  (* TODO(unnonouno): Too complicated. Make it simple! *)
  let func = Printf.sprintf
    "pfi::lang::function<%s(%s, %s)>" r r r in
  func ^ "(&" ^ agg ^ ")"
;;

let gen_keeper_register m ret_type =
  let arg_types = List.map (fun f -> f.field_type) (List.tl m.method_arguments) in
  let method_name_str = gen_string_literal m.method_name in
  let routing, _, agg = get_decorator m in
  match routing with
  | Random ->
    let func = gen_template "k.register_async_random" (ret_type::arg_types) in
    let call = gen_call func [method_name_str] in
    [ (0, call) ]

  | Cht i ->
    let args = List.tl arg_types in
    let arg_strs = List.map gen_type (ret_type::args) in
    (* TODO(unnonouno): Is this number really required to be a template argument? *)
    let num = string_of_int i in
    let func = gen_template_with_strs "k.register_async_cht" (num::arg_strs) in
    let call = gen_call func [method_name_str; gen_aggregator_function ret_type agg] in
    [ (0, call) ]

  | Broadcast ->
    let func = gen_template "k.register_async_broadcast" (ret_type::arg_types) in
    let call = gen_call func [method_name_str; gen_aggregator_function ret_type agg] in
    [ (0, call) ]

  | Internal -> (* no code generated in keeper *)
    []
;;

let gen_keeper_method m =
  match m.method_return_type with
  | Some ret_type ->
    gen_keeper_register m ret_type
  | None ->
    (* TODO(unnonouno): How to treat funcitons that return no values? *)
    []
;;

let gen_keeper s =
  List.map gen_keeper_method s.service_methods
;;

let gen_keeper_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_keeper.cpp" in

  let name_str = gen_string_literal base in
  let servers = List.concat (List.map gen_keeper services) in

  let namespace = parse_namespace conf.Config.namespace in
  let func = String.concat "::" namespace ^ "::run_keeper" in
  
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
      (0, gen_jubatus_include conf "framework/aggregators.hpp");
      (0, gen_jubatus_include conf "framework/keeper.hpp");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (
      List.concat [
        [
          (0, "int run_keeper(int argc, char* argv[]) {");
          (1,   "try {");
          (2,     "jubatus::server::framework::keeper k(");
          (4,         "jubatus::server::framework::keeper_argv(argc, argv, " ^ name_str ^ "));");
        ];
        indent_lines 2 (List.concat servers);
        [
          (2,     "return k.run();");
          (1,   "} catch (const jubatus::exception::jubatus_exception& e) {");
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

let gen_impl_method m =
  let name = m.method_name in
  let args_def = gen_function_args_def m.method_arguments in
  (* Do not use the first argument. It is used for keepers. *)
  let args = List.map (fun f -> f.field_name) (List.tl m.method_arguments) in
  let ret_type = gen_ret_type m.method_return_type in

  let _, request, _ = get_decorator m in
  let lock_type =
    match request with
    | Update -> "JWLOCK_"
    | Analysis -> "JRLOCK_"
    | Nolock -> "NOLOCK_" in
  let lock = gen_call lock_type ["p_"] in
  (* TODO(unnonouno): think of generating this abnormal method, which calls the method of p_ rather than get_p(). *)
  let pointer =
    if name = "get_status" then
      "p_"
    else
      "get_p()"
  in
  let call =
    match m.method_return_type with
    | None ->
      gen_call (pointer ^ "->" ^ name) args
    | Some typ ->
      gen_call ("return " ^ pointer ^ "->" ^ name) args
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

let gen_impl s =
  let methods = List.map gen_impl_method s.service_methods in
  let name = s.service_name in
  let impl_name = name ^ "_impl_" in
  let serv_name = name ^ "_serv" in
  let use_cht = include_cht_method s in
  List.concat [
    [
      (0, "class " ^ impl_name ^ " : public " ^ name ^ "<" ^ impl_name ^ "> {");
      (0, " public:");
      (1,   "explicit " ^ impl_name ^ "(const jubatus::server::framework::server_argv& a):");
      (2,     name ^ "<" ^ impl_name ^ ">(a.timeout),");
      (2,     "p_(new jubatus::server::framework::server_helper<" ^ serv_name ^ ">(a, " ^ gen_bool_literal use_cht ^ ")) {");
      (1,   "}")
    ];
    indent_lines 1 (concat_blocks methods);
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

let gen_impl_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_impl.cpp" in
  let name_str = gen_string_literal base in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let impls = List.map gen_impl services in
  let s = concat_blocks [
    [
      (0, "#include <map>");
      (0, "#include <string>");
      (0, "#include <vector>");
      (0, "#include <utility>");
      (0, "#include <pficommon/lang/shared_ptr.h>");
      (0, "");
      (0, gen_jubatus_include conf "framework.hpp");
      (0, "#include \"" ^ base ^ "_server.hpp\"");
      (0, "#include \"" ^ base ^ "_serv.hpp\"");
    ];
    make_namespace namespace (List.concat impls);
    [
      (0, "int main(int argc, char* argv[]) {");
      (1,   "return");
      (* TODO(unnonouno): does not work when service name is not equal to a source file*)
      (2,     "jubatus::server::framework::run_server<jubatus::server::" ^ base ^ "_impl_>");
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

let gen_server_template_header_method m =
  let name = m.method_name in
  let args_def = gen_function_reference_args_def (List.tl m.method_arguments) in
  let ret_type = gen_ret_type m.method_return_type in
  let const = gen_const m in
  [ (0, Printf.sprintf "%s %s%s%s;" ret_type name args_def const) ]
;;

(* TODO(unnonouno): These special methods are going to be removed from IDL *)
let filter_methods methods =
  List.filter (fun m ->
    not (m.method_name = "save"
         or m.method_name = "load"
         or m.method_name = "get_status")
  ) methods
;;

let gen_server_template_header s =
  let ms = filter_methods s.service_methods in
  let methods = List.map gen_server_template_header_method ms in
  let name = s.service_name in
  let serv_name = name ^ "_serv" in
  List.concat [
    [
      (0, "class " ^ serv_name ^ " : public jubatus::server::framework::server_base {  // do not change");
      (0, " public:");
      (1,   serv_name ^ "(");
      (2,     "const jubatus::server::framework::server_argv& a,");
      (2,     "const pfi::lang::shared_ptr<common::lock_service>& zk);  // do not change");
      (1,   "virtual ~" ^ serv_name ^ "();  // do not change");
      (0,   "");
      (1,   "virtual mixer::mixer* get_mixer() const;");
      (1,   "pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const;");
      (1,   "void get_status(status_t& status) const;");
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

let gen_server_template_header_file conf source services =
  let base = File_util.take_base source in
  let filename = base ^ "_serv.tmpl.hpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let servers = List.map gen_server_template_header services in

  let content = concat_blocks [
    [
      (0, gen_jubatus_include conf "framework.hpp");
      (0, "#include \"" ^ base ^ "_types.hpp\"");
    ];
    make_namespace namespace (concat_blocks servers)
  ] in
  
  make_template_header conf source filename content
;;

let gen_server_template_source_method s m =
  let serv_name = s.service_name ^ "_serv" in

  let name = m.method_name in
  let args_def = gen_function_reference_args_def (List.tl m.method_arguments) in
  let ret_type = gen_ret_type m.method_return_type in
  let const = gen_const m in
  [
    (0, Printf.sprintf "%s %s::%s%s%s {" ret_type serv_name name args_def const);
    (0, "}");
  ]
;;

let gen_server_template_source s =
  let ms = filter_methods s.service_methods in
  let methods = List.map (gen_server_template_source_method s) ms in
  let name = s.service_name in
  let serv_name = name ^ "_serv" in
  concat_blocks [
    [
      (0, serv_name ^ "::" ^ serv_name ^ "(");
      (1,   "const jubatus::server::framework::server_argv& a,");
      (1,   "const pfi::lang::shared_ptr<common::lock_service>& zk)");
      (2,     ": jubatus::server::framework::server_base(a) {");
      (1,   "// somemixable* mi = new somemixable;");
      (1,   "// somemixable_.set_model(mi);");
      (1,   "// get_mixable_holder()->register_mixable(mi);");
      (0, "}");
      (0, "");
      (0, serv_name ^ "::~" ^ serv_name ^ "() {");
      (0, "}");
      (0, "");
      (0, "virtual mixer::mixer* " ^ serv_name ^ "::get_mixer() const {");
      (0, "}");
      (0, "");
      (0, "pfi::lang::shared_ptr<framework::mixable_holder> "
        ^ serv_name ^ "::get_mixable_holder() const {");
      (0, "}");
      (0, "");
      (0, "void " ^ serv_name ^ "::get_status(status_t& status) const {");
      (0, "}");
      (0, "");
    ];
    concat_blocks methods;
  ]
;;

let gen_server_template_source_file conf source services =
  let base = File_util.take_base source in
  let serv = base ^ "_serv" in
  let serv_hpp = base ^ "_serv.hpp" in
  let filename = serv ^ ".tmpl.cpp" in

  let namespace = parse_namespace conf.Config.namespace in
  let namespace = List.append namespace ["server"] in
  let servers = List.map gen_server_template_source services in

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

  gen_type_file conf source idl;
  gen_client_file conf source services;
  gen_server_file conf source services;
  gen_keeper_file conf source services;
  gen_impl_file conf source services;
  if conf.Config.default_template then begin
    gen_server_template_header_file conf source services;
    gen_server_template_source_file conf source services;
  end
;;

let generate_client conf source idl =
  let services = get_services idl in

  gen_type_file conf source idl;
  gen_client_file conf source services;
;;
