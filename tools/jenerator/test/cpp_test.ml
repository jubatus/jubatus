open OUnit
open Cpp
open Syntax

let _ = run_test_tt_main begin "cpp.ml" >::: [

  "test_parse_namespace" >:: begin fun() ->
    assert_equal ["fuga"; "hoge"] (parse_namespace "fuga::hoge")
  end;

  "test_gen_template" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    assert_equal
      "t<bool, std::string>"
      (gen_template names "t" [Bool; String]);

    assert_equal
      "t<std::vector<bool> >"
      (gen_template names "t" [List Bool])
  end;

  "test_gen_type" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    Hashtbl.add names "t" "name::t";
    assert_equal
      "name::t"
      (gen_type names (Struct "t"))
  end;

  "test_gen_argument_type" >:: begin fun() ->
    let names = Hashtbl.create 10 in
    let gen = gen_argument_type names in
    assert_equal "const std::string&" (gen String);
    assert_equal "const std::vector<bool>&" (gen (List Bool));
    assert_equal "const std::map<bool, std::string>&" (gen (Map(Bool, String)));
    assert_equal "const MyType&" (gen (Struct "MyType"));
    assert_equal "bool" (gen Bool);
    assert_equal "int8_t" (gen (Int(true, 1)));
    assert_equal "float" (gen (Float false));
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
