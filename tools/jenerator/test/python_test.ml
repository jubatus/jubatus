open OUnit
open Python
open Syntax

let _ = run_test_tt_main begin "python.ml" >::: [

  "test_gen_def" >:: begin fun() ->
    assert_equal
      "def f(self):"
      (gen_def "f" []);
    assert_equal
      "def f(self, x, y, z):"
      (gen_def "f" ["x"; "y"; "z"]);
  end;

  "test_gen_args" >:: begin fun() ->
    assert_equal
      "()"
      (gen_args []);
    assert_equal
      "(x, y)"
      (gen_args ["x"; "y"]);
  end;

] end
