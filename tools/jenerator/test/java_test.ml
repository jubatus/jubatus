open OUnit
open Java
open Syntax

let _ = run_test_tt_main begin "java.ml" >::: [

  "test_rename_without_underbar" >:: begin fun() ->
    assert_equal
      "HogeFugaFoo"
      (rename_without_underbar "hoge_fuga_foo");
    assert_equal
      "A"
      (rename_without_underbar "a__");
    assert_equal
      "A"
      (rename_without_underbar "_a")
  end;

  "test_include_list" >:: begin fun() ->
    assert_equal
      true
      (include_list (List Bool));
    assert_equal
      true
      (include_list (Map(Bool, List Bool)))
  end;

  "test_gen_type" >:: begin fun() ->
    assert_equal
      "int"
      (gen_type (Int(true, 4)));

    assert_equal
      "long"
      (gen_type (Int(true, 8)));

    assert_equal
      "boolean"
      (gen_type Bool);

    assert_equal
      "float"
      (gen_type (Float false));

    assert_equal
      "double"
      (gen_type (Float true));

    assert_equal
      "String"
      (gen_type String);

    assert_equal
      "List<Float >"
      (gen_type (List(Float false)));

    assert_equal
      "Map<String, Integer >"
      (gen_type (Map(String, Int(true, 4))));

    assert_equal
      "MyObject"
      (gen_type (Struct("my_object")));
  end;

  "test_gen_object_type" >:: begin fun() ->
    assert_equal
      "Integer"
      (gen_object_type (Int(true, 4)));

    assert_equal
      "Long"
      (gen_object_type (Int(true, 8)));

    assert_equal
      "Boolean"
      (gen_object_type Bool);

    assert_equal
      "Float"
      (gen_object_type (Float false));

    assert_equal
      "Double"
      (gen_object_type (Float true));

    assert_equal
      "String"
      (gen_object_type String);

    assert_equal
      "List<Float >"
      (gen_object_type (List(Float false)));

    assert_equal
      "Map<String, Integer >"
      (gen_object_type (Map(String, Int(true, 4))));
  end;

  "test_gen_ret_type" >:: begin fun() ->
    assert_equal
      "void"
      (gen_ret_type None)
  end;

  "test_gen_args" >:: begin fun() ->
    assert_equal
      "()"
      (gen_args []);

    assert_equal
      "(x)"
      (gen_args ["x"]);

    assert_equal
      "(x, y)"
      (gen_args ["x"; "y"]);
  end;

  "test_gen_args_with_type" >:: begin fun() ->
    assert_equal
      "()"
      (gen_args_with_type []);

    assert_equal
      "(String x)"
      (gen_args_with_type [("x", String)]);

    assert_equal
      "(String x, float y)"
      (gen_args_with_type [("x", String); ("y", Float(false))]);
  end;

  "test_gen_call" >:: begin fun() ->
    assert_equal
      "f(10);"
      (gen_call "f" ["10"]);
  end;

  "test_gen_return" >:: begin fun() ->
    assert_equal
      "return iface_.f();"
      (gen_return "f" []);

    assert_equal
      "return iface_.f(x, y);"
      (gen_return "f" ["x"; "y"]);
  end;

] end

