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

  "test_gen_bool_literal" >:: begin fun() ->
    assert_equal
      "True"
      (gen_bool_literal true);
    assert_equal
      "False"
      (gen_bool_literal false);
  end;

  "test_gen_args" >:: begin fun() ->
    assert_equal
      "()"
      (gen_args []);
    assert_equal
      "(x, y)"
      (gen_args ["x"; "y"]);
  end;

  "test_gen_call" >:: begin fun() ->
    assert_equal
      "fun(x, y)"
      (gen_call "fun" ["x"; "y"])
  end;

  "test_gen_list" >:: begin fun() ->
    assert_equal
      "[]"
      (gen_list []);
    assert_equal
      "[x, y]"
      (gen_list ["x"; "y"]);
  end;

  "test_gen_type" >:: begin fun() ->
    assert_equal
      "TObject()"
      (gen_type Object);
    assert_equal
      "TBool()"
      (gen_type Bool);
    assert_equal
      "TFloat()"
      (gen_type (Float true));
    assert_equal
      "TMap(TString(), TInt(True, 4))"
      (gen_type (Map(String, Int(true, 4))));
  end;

  "test_gen_client_call" >:: begin fun() ->
    assert_equal
      "self.jubatus_client.call(\"fun\", [x, y], TString(), [TBool(), TFloat()])"
      (gen_client_call {
        method_name = "fun";
        method_return_type = Some String;
        method_arguments = [
          { field_number = 0; field_type = Bool; field_name = "x" };
          { field_number = 1; field_type = Float false; field_name = "y" };
        ];
        method_decorators = []
       })
  end;

] end
