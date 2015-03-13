open OUnit
open Syntax

type result =
  | Success of idl
  | LexError of int * int * char
  | ParseError of int * int
  [@@deriving show]

let parse program =
  let lexbuf = Lexing.from_string program in
  try
    Success(Parse.parse lexbuf)
  with
  | Parse.Syntax_error pos ->
    Lexing.(
      ParseError(pos.pos_lnum, pos.pos_cnum - pos.pos_bol)
    )
  | Jdl_lexer.Illegal_character(pos, ch) ->
    Lexing.(
      LexError(pos.pos_lnum, pos.pos_cnum - pos.pos_bol, ch)
    )
;;

let parse_test program expected =
  assert_equal ~printer:show_result expected @@ parse program
;;

let parse_error_position_test program lnum cnum =
  parse_test program @@ ParseError(lnum, cnum)
;;

let lex_error_position_test program lnum cnum ch =
  parse_test program @@ LexError(lnum, cnum, ch)
;;

let parse_success_test program idl =
  parse_test program @@ Success(idl)

let _ = run_test_tt_main begin "parser" >::: [
  "test_parse_error_position" >:: begin fun() ->
    parse_error_position_test "" 1 0;
    parse_error_position_test "  here" 1 2;
    parse_error_position_test "# comment\n here" 2 1
  end;

  "test_illegal_character" >:: begin fun() ->
    lex_error_position_test "  $" 1 2 '$'
  end;

  "test_parse_success" >:: begin fun() ->
    parse_success_test "%include \"test\"" [Include("test")]
  end
] end
