open OUnit
open Lib

let assert_equal = OUnit.assert_equal ~printer: Std.dump;;

let _ = run_test_tt_main begin "lib.ml" >::: [

  "test_snake_to_lower" >:: begin fun() ->
    assert_equal "" (snake_to_lower "");
    assert_equal "abc" (snake_to_lower "abc");
    assert_equal "abcDef" (snake_to_lower "abc_def");
    assert_equal "abcDefGhi" (snake_to_lower "abc_def_ghi");
  end;

  "test_snake_to_upper" >:: begin fun() ->
    assert_equal "" (snake_to_upper "");
    assert_equal "Abc" (snake_to_upper "abc");
    assert_equal "AbcDef" (snake_to_upper "abc_def");
    assert_equal "AbcDefGhi" (snake_to_upper "abc_def_ghi");
  end;

] end
