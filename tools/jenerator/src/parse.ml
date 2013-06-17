exception Syntax_error of Lexing.position

let parse lexbuf =
  try
    Jdl_parser.input Jdl_lexer.token lexbuf
  with
  | Parsing.Parse_error ->
    let p = Lexing.lexeme_start_p lexbuf in
    raise (Syntax_error p)
;;
