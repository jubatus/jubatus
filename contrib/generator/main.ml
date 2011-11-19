(*
  main.ml for source code generator
  there are many TODOs
  - parse 'public:' 'private:' notations
  - multiple language output to use in pipe4
  - put class under namespaces?
  - do we support class templates?
  - do we support class-method templates?
*)

open Stree;;

let get_name filename =
  let start = try 1 + String.rindex filename '/' with _ -> 0 in
  let fin = String.rindex filename '.' in
  String.sub filename start (fin - start);;

let replace_suffix suffix filename =
  let base = String.sub filename 0 (String.length filename - 2) in
  base ^ "." ^ suffix;;

let _ =
  let source_file = Sys.argv.(1) in
(*  let output_file = replace_suffix "cpp" source_file in
  print_endline (source_file ^ " ===> " ^ output_file); *)
  let lexbuf = Lexing.from_channel (open_in source_file) in
  try
    let (includee,classimpls) = Parser.input Lexer.token lexbuf in
    print_endline "------------- syntax tree:";
    List.iter (fun s -> print_endline ("including => "^s) ) includee;
    List.iter Stree.print_classimpl classimpls;
    Stree.print_all();
    print_endline "------------- generated code:";
(*    let m = new Generator.jubatus_module "jupatus" (get_name source_file) includee classimpls in *)
(*    m#generate *)
    ()
  with
    | Parsing.Parse_error ->
      let p = Lexing.lexeme_start_p lexbuf in
      Printf.fprintf stderr
        "File \"%s\", line %d, character %d: syntax error.\n"
        p.Lexing.pos_fname p.Lexing.pos_lnum
        (p.Lexing.pos_cnum - p.Lexing.pos_bol)
    | exn ->
      print_endline (Printexc.to_string exn);;
