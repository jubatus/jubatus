(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

let outdir  = ref "."
let internal = ref false
let namespace = ref "jubatus"
let default_template = ref false
let debugmode = ref false

let _ =
  Arg.parse [
    ("-o", Arg.Set_string(outdir), "output directory"); (* FIXME: bug: ignored *)
    ("-i", Arg.Set(internal), "internal include"); (* for jubatus internal use *)
    ("-n", Arg.Set_string(namespace), "namespace"); (* C++ namespace *)
    ("-t", Arg.Set(default_template), "output default template xxx_serv file");
    ("-d", Arg.Set(debugmode),        "debug (verbose) mode");
  ] (fun _ -> ()) "usage:";;

(* Now they are fixed, so remove the refs and make them non-writable!!! *)
let outdir  = !outdir
let internal = !internal
let namespace = !namespace
let default_template = !default_template
let debugmode = !debugmode

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
      raise exn;;

let make_output filename =
  if debugmode then stdout
  else begin
    print_endline ("generate ==> " ^ filename);
    open_out filename
  end;;

let _ =
  let source_file = Sys.argv.(1) in

  (* parse the idl *)
  let strees = parse source_file in

  (* validation
     - proper decorator ?
     - proper artument ?
       - cht -> string
       - first argv must be string
       - proper aggragator ?
       - have basic methods with proper type ? (save, load, get_status)  *)
  Validator.check strees;

  (* output -> xxx_keeper.cpp xxx_serv.hpp xxx_impl.cpp *)

  let basename = Util.get_basename source_file in
  let s = new Generator.spec namespace internal source_file basename in

  Generator.generate_keeper s (make_output (basename^"_keeper.cpp")) strees;

  Generator.generate_impl s (make_output (basename^"_impl.cpp")) strees;
  
  if default_template then begin
    Generator.generate_server_tmpl_header s (make_output (basename^"_serv.tmpl.hpp")) strees;
    Generator.generate_server_tmpl_impl s (make_output (basename^"_serv.tmpl.cpp")) strees
  end;;
