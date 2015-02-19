(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2013 Preferred Networks and Nippon Telegraph and
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

let show_error_message file pos message =
  Printf.fprintf stderr
    "File \"%s\", line %d, character %d: %s\n"
    file pos.Lexing.pos_lnum
    (pos.Lexing.pos_cnum - pos.Lexing.pos_bol)
    message
;;

let parse source_file =
  let lexbuf = Lexing.from_channel (open_in source_file) in
  try
    Parse.parse lexbuf
  with
  | Jdl_lexer.Illegal_character (p, ch) ->
    let message = Printf.sprintf "illegal character: '%c'." ch in
    show_error_message source_file p message;
    raise Parsing.Parse_error;
  | Parse.Syntax_error p ->
    show_error_message source_file p "syntax error.";
    raise Parsing.Parse_error;
  | exn ->
    print_endline (Printexc.to_string exn);
    raise exn
;;

let _ =
  let (conf, files) = Config.parse_args () in
  let generate =
    match conf.Config.language with
    | "server" -> Cpp.generate_server
    | "cpp" -> Cpp.generate_client
    | "python" -> Python.generate
    | "java" -> Java.generate
    | "ruby" -> Ruby.generate
    | "go" -> Go.generate
    | _ ->
      print_endline ("Unsupported language: \"" ^ conf.Config.language ^ "\"");
      exit 1 in

  List.iter (fun source_file ->
    let idl = parse source_file in
    generate conf source_file idl
  ) files
;;

  (* validation
     - proper decorator ?
     - proper artument ?
       - cht -> string
       - first argv must be string
       - proper aggragator ?
       - have basic methods with proper type ? (save, load, get_status)  *)
