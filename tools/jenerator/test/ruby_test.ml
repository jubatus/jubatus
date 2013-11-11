open OUnit
open Ruby
open Syntax

let assert_equal = OUnit.assert_equal ~printer: Std.dump;;

let _ = run_test_tt_main begin "ruby.ml" >::: [

  "test_gen_list" >:: begin fun() ->
    assert_equal "[]" (gen_list []);
    assert_equal "[1]" (gen_list ["1"]);
    assert_equal "[1, 2]" (gen_list ["1"; "2"]);
  end;

  "test_gen_type" >:: begin fun () ->
    assert_equal "TObject.new" (gen_type Object);
    assert_equal "TList.new(TBool.new)" (gen_type (List Bool));
    assert_equal "TMap.new(TString.new, TFloat.new)" (gen_type (Map(String, Float false)));
  end;

] end
