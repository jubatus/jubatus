open OUnit
open Syntax

let parse_error_position_test program line character =
  let lexbuf = Lexing.from_string program in
  try
    let _ = Parse.parse lexbuf in
    assert_failure "This IDL must cause an error."
  with
  | Parse.Syntax_error pos ->
    assert_equal line pos.Lexing.pos_lnum;
    assert_equal character (pos.Lexing.pos_cnum - pos.Lexing.pos_bol)
;;

let _ = run_test_tt_main begin "parser" >::: [
  "test_parse_error_position" >:: begin fun() ->
    parse_error_position_test "  here" 1 2;
    parse_error_position_test "# comment\n here" 2 1
  end;

  "test_illegal_character" >:: begin fun() ->
    let lexbuf = Lexing.from_string "  $" in
    try
      let _ = Parse.parse lexbuf in
      assert_failure "This IDL must cause an error."
    with
    | Jdl_lexer.Illegal_character(pos, ch) ->
      assert_equal 1 pos.Lexing.pos_lnum;
      assert_equal 2 (pos.Lexing.pos_cnum - pos.Lexing.pos_bol);
      assert_equal '$' ch
  end;
] end
