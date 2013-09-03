open OUnit
open Ruby
open Syntax

let _ = run_test_tt_main begin "cpp.ml" >::: [

  "test_gen_retval'" >:: begin fun() ->
    assert_equal "@cli.call(:func)" (gen_retval' "func" []);
    assert_equal "@cli.call(:func, arg1)" (gen_retval' "func" ["arg1"]);
    assert_equal "@cli.call(:func, arg1, arg2)" (gen_retval' "func" ["arg1"; "arg2"]);
  end;

  "test_gen_def" >:: begin fun() ->
    assert_equal "def func" (gen_def "func" []);
    assert_equal "def func(arg1)" (gen_def "func" ["arg1"]);
    assert_equal "def func(arg1, arg2)" (gen_def "func" ["arg1"; "arg2"])
  end;

] end
