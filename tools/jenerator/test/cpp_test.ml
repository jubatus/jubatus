open OUnit
open Cpp
open Syntax

let assert_equal = OUnit.assert_equal ~printer: Std.dump;;

let _ = run_test_tt_main begin "cpp.ml" >::: [

  "test_parse_namespace" >:: begin fun() ->
    assert_equal ["fuga"; "hoge"] (parse_namespace "fuga::hoge")
  end;

  "test_gen_template" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    assert_equal
      "t<bool, std::string>"
      (gen_template names false "t" [Bool; String]);

    assert_equal
      "t<std::vector<bool> >"
      (gen_template names false "t" [List Bool]);
  end;

  "test_gen_type" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    Hashtbl.add names "t" "name::t";
    assert_equal
      "name::t"
      (gen_type names false (Struct "t"));

    assert_equal "jubatus::client::common::datum" (gen_type names false Datum);
    assert_equal "jubatus::core::fv_converter::datum" (gen_type names true Datum);
  end;

  "test_gen_argument_type" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    let gen = gen_argument_type names false in
    assert_equal "const std::string&" (gen String);
    assert_equal "const std::vector<bool>&" (gen (List Bool));
    assert_equal "const std::map<bool, std::string>&" (gen (Map(Bool, String)));
    assert_equal "const MyType&" (gen (Struct "MyType"));
    assert_equal "bool" (gen Bool);
    assert_equal "int8_t" (gen (Int(true, 1)));
    assert_equal "float" (gen (Float false));
    assert_equal "const jubatus::client::common::datum&" (gen Datum);

    assert_equal "const jubatus::core::fv_converter::datum&" (gen_argument_type names true Datum);
  end;

  "test_gen_string_literal" >:: begin fun() ->
    assert_equal
      "\"saitama\""
      (gen_string_literal "saitama");
    assert_equal
      "\"`~!@#$%^&*()-_=+[{]}\\\\|;:'\\\"\""
      (gen_string_literal "`~!@#$%^&*()-_=+[{]}\\|;:'\"")
  end;

  "test_gen_args" >:: begin fun() ->
    assert_equal
      "()"
      (gen_args []);
    assert_equal
      "(saitama)"
      (gen_args ["saitama"]);
    assert_equal
      "(saitama, gumma)"
      (gen_args ["saitama"; "gumma"])
  end;

] end
