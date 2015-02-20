(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2015 Preferred Networks and Nippon Telegraph and
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

open Syntax
open Lib

let comment_out_head = "//"

(* Golang-ish identifier converter used instead of snake_to_upper *)
let snake_to_go_ident s =
  let ss = Str.split (Str.regexp "_") s in
  String.concat "" (List.map (fun x ->
    match x with
    | "id" -> "ID"
    | _ -> String.capitalize x) ss)

let make_header conf source filename content =
  make_source conf source filename content comment_out_head
;;

let gen_string_literal s =
  "\"" ^ String.escaped s ^ "\""
;;

let gen_bool_literal = function
  | true -> "true"
  | false -> "false"
;;

let gen_args args =
  "(" ^ String.concat ", " args ^ ")"
;;

let gen_call func args =
  (* TODO(unnonouno): format for long lines *)
  func ^ gen_args args
;;

let gen_list vars =
  "[" ^ String.concat ", " vars ^ "]"
;;

let gen_msgpack_arg_list args =
  "codec.MsgpackSpecRpcMultiArgs{" ^ String.concat ", " args ^ "}"
;;

let gen_client_struct module_name =
  [
    (0, "type " ^ (snake_to_go_ident module_name) ^ "Client struct {");
    (2,     "client rpc.Client");
    (2,     "name   string");
    (0, "}");
  ]
;;

let rec gen_type = function
  | Object -> "new(interface{})"
  | Bool -> "bool"
  | Int(signed, bytes) -> begin
    match signed, bytes with
    | _, 1 -> "int8"
    | _, 2 -> "int16"
    | _, 4 -> "int32"
    | _, 8 -> "int64"
    | _ -> raise (Unknown_type (Printf.sprintf "unknown int type: %b, %d" signed bytes))
  end
  | Float(_) -> "float64"
  | Raw -> "[]bytes"
  | String -> "string"
  | Datum -> "common.Datum"
  | Struct s  -> snake_to_go_ident s
  | List t -> "[]" ^ gen_type t
  | Map(key, value) -> "map[" ^ gen_type key ^ "]" ^ (gen_type value)
  | Nullable(t) -> gen_call "TNullable.new" [gen_type t]
;;

let gen_client_call m =
  let name = m.method_name in
  let args = List.map (fun f -> f.field_name) m.method_arguments in
  let args' = gen_msgpack_arg_list ("c.name" ::  args) in
  let ret_type' = "&result" in
  let call_arg = [gen_string_literal name; args'; ret_type';] in
  gen_call "c.client.Call" call_arg
;;

let gen_result m =
  Option.default "" (Option.map (fun f -> "var result " ^ gen_type f) m.method_return_type)
;;

let gen_def service func args return_type =
  let arg_list = List.map (fun a -> a.field_name ^ " " ^ gen_type a.field_type) args in
  "func (c *" ^ service ^ "Client) " ^
    snake_to_go_ident func ^ "(" ^
      String.concat ", "
                    arg_list ^
      ") " ^ return_type ^ " {"
;;

let gen_constructor service =
  let camel_name = snake_to_go_ident service in
  [ (0, "func New" ^ camel_name ^ "Client(host string, name string) (*" ^
          camel_name ^ "Client, error) {");
    (2,   "conn, err := net.Dial(\"tcp\", host)");
    (2,   "if err != nil {");
    (4,   "return nil, err");
    (2,   "}");
    (2,   "mh := new(codec.MsgpackHandle)");
    (2,   "mh.StructToArray = true");
    (2,   "rpcCodec := codec.MsgpackSpecRpc.ClientCodec(conn, mh)");
    (2,   "client := rpc.NewClientWithCodec(rpcCodec)");
    (2,   "return &" ^ camel_name ^ "Client{*client, name}, nil");
    (0, "}");
  ]
;;

let gen_return_type m =
  Option.default "" (Option.map gen_type m.method_return_type)
;;

let gen_client_method service_name m =
  let name = m.method_name in
  [ (0, gen_def (snake_to_go_ident service_name) name m.method_arguments (gen_return_type m));
    (2,   gen_result m);
    (2,   gen_client_call m);
    (2,   "return result");
    (0, "}");
  ]
;;

let gen_client s =
  let gen_client_with_service = gen_client_method s.service_name in
  let methods = List.map gen_client_with_service s.service_methods in
  let content = methods in
  concat_blocks content;
;;


let gen_self_with_equal field_names =
  List.map (fun s -> (0, "@" ^ s ^ " = " ^ s)) field_names
;;

let gen_message m =
  let fields = List.map (fun f -> (f.field_name, f.field_type) ) m.message_fields in
  List.concat [
      [(0, "type " ^ snake_to_go_ident m.message_name ^ " struct {")];
      List.map (fun (name, type_name) ->
                (2, snake_to_go_ident name ^ " " ^ gen_type type_name) ) fields;
      [(0, "}")]
  ]
;;

let gen_typedef = function
  | Message m ->
     gen_message m
  | _ ->
     [(0, "")]
;;

let common_functions name =
  [
    (* save func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) Save(id string) bool {");
    (2,   "var result bool");
    (2,   "c.client.Call(\"save\", codec.MsgpackSpecRpcMultiArgs{c.name, id}, &result)");
    (2,   "return result");
    (0, "}");
    (0, "");

    (* load func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) Load(id string) bool {");
    (2,   "var result bool");
    (2,   "c.client.Call(\"load\", codec.MsgpackSpecRpcMultiArgs{c.name, id}, &result)");
    (2,   "return result");
    (0, "}");
    (0, "");

    (* get_config *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) GetConfig() map[string]map[string]string {");
    (2,   "var result map[string]map[string]string");
    (2,   "c.client.Call(\"get_config\", codec.MsgpackSpecRpcMultiArgs{c.name}, &result)");
    (2,   "return result");
    (0, "}");
    (0, "");

    (* do_mix func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) DoMix() bool {");
    (2,   "var result bool");
    (2,   "c.client.Call(\"do_mix\", codec.MsgpackSpecRpcMultiArgs{c.name}, &result)");
    (2,   "return result");
    (0, "}");
    (0, "");

    (* get_name func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) GetName() string {");
    (2,   "return c.name");
    (0, "}");
    (0, "");

    (* set_name func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) SetName(new_name string) {");
    (2,   "c.name = new_name");
    (0, "}");
    (0, "");

    (* get_client func *)
    (0, "func (c *" ^ snake_to_go_ident name ^ "Client) GetClient() rpc.Client {");
    (2,   "return c.client");
    (0, "}");
  ]
;;

let gen_client_file conf source services =
  let base = File_util.take_base source in
  let filename = Filename.concat base "client.go" in
  let clients = List.map (fun s ->
    concat_blocks [
      gen_client_struct s.service_name;
      gen_constructor s.service_name;
      gen_client s;
      common_functions s.service_name;
    ]
  ) services in

  let content = concat_blocks [
    [ (0, "package jubatus_client")];
    [
      (0, "import (");
      (2,   "common \"github.com/jubatus/jubatus-go-client/lib/common\"");
      (2,   "\"github.com/ugorji/go/codec\"");
      (2,   "\"net\"");
      (2,   "\"net/rpc\"");
      (0, ")");
    ];
    concat_blocks clients;
  ] in
  make_header conf source filename content
;;

let gen_type_file conf source idl =
  let base = File_util.take_base source in
  let name = Filename.concat base "types.go" in
  let types = List.map gen_typedef idl in

  let content = concat_blocks [
    [ (0, "package jubatus_client");
      (0, "import (");
      (2,   "common \"github.com/jubatus/jubatus-go-client/lib/common\"");
      (0, ")");
    ];
    concat_blocks types;
  ] in
  make_header conf source name content
;;

let generate conf source idl =
  let services = get_services idl in
  gen_client_file conf source services;
  gen_type_file conf source idl
;;
