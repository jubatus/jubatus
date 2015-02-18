(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.

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

let outdir = ref "."
let debugmode = ref false

let _ =
  Arg.parse [
    ("-o", Arg.Set_string(outdir), "output directory");
    ("-d", Arg.Set(debugmode),     "debug (verbose) mode");
  ] (fun _ -> ()) "usage:";;

let outdir  = !outdir
let debugmode = !debugmode

let parse source_file =
  let lexbuf = Lexing.from_channel (open_in source_file) in
  try
    List.rev (Jubadoc_parser.input Jubadoc_lexer.token lexbuf)
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

  let basename = Util.get_basename source_file in
  let s = new Generator.spec source_file basename in

  Rst_generator.generate s (make_output (outdir^"/"^basename^".rst")) strees;;
