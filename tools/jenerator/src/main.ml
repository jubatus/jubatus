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

let parse source_file =
  let lexbuf = Lexing.from_channel (open_in source_file) in
  try
    Jdl_parser.input Jdl_lexer.token lexbuf
  with
  | Parsing.Parse_error ->
    let p = Lexing.lexeme_start_p lexbuf in
    Printf.fprintf stderr
      "File \"%s\", line %d, character %d: syntax error.\n"
      source_file p.Lexing.pos_lnum
      (p.Lexing.pos_cnum - p.Lexing.pos_bol);
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
    (*| "java" -> Java.generate*)
    | "ruby" -> Ruby.generate
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
