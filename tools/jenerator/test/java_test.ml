open OUnit
open Java
open Syntax

let assert_equal = OUnit.assert_equal ~printer: Std.dump;;

let _ = run_test_tt_main begin "java.ml" >::: [

  "test_include_list" >:: begin fun() ->
    assert_equal
      true
      (include_list (List Bool));
    assert_equal
      true
      (include_list (Map(Bool, List Bool)))
  end;

  "test_include_map" >:: begin fun() ->
    assert_equal
      true
      (include_map (Map(Bool, Bool)));
    assert_equal
      true
      (include_map (List(Map(Bool, Bool))))
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
      "Datum"
      (gen_type Datum);

    assert_equal
      "List<Float>"
      (gen_type (List(Float false)));

    assert_equal
      "Map<String, Integer>"
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
      "List<Float>"
      (gen_object_type (List(Float false)));

    assert_equal
      "Map<String, Integer>"
      (gen_object_type (Map(String, Int(true, 4))));
  end;

  "test_split_namespace" >:: begin fun() ->
    assert_equal
      []
      (split_namespace { Config.namespace = "";
                         Config.language = "java";
                         Config.outdir = "";
                         Config.internal = false;
                         Config.default_template = false;
                         Config.include_guard = "";
                         Config.idl_version = None; } );
    assert_equal
      ["us"]
      (split_namespace { Config.namespace = "us";
                         Config.language = "java";
                         Config.outdir = "";
                         Config.internal = false;
                         Config.default_template = false;
                         Config.include_guard = "";
                         Config.idl_version = None; } );
    assert_equal
      ["us"; "jubat"; "classifier"]
      (split_namespace { Config.namespace = "us.jubat.classifier";
                         Config.language = "java";
                         Config.outdir = "";
                         Config.internal = false;
                         Config.default_template = false;
                         Config.include_guard = "";
                         Config.idl_version = None; } );
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

  "test_gen_call" >:: begin fun() ->
    assert_equal
      "f(10)"
      (gen_call "f" ["10"]);
  end;

  "test_gen_public" >:: begin fun() ->
    let f = {
      field_number = 1;
      field_type = Int(true, 4);
      field_name = "x";
    } in
    assert_equal
      [ (0, "public String fun(int x) {");
        (1,   "y = x;");
        (0, "}"); ]
      (gen_public (Some String) "fun" [f] "" [(1, "y = x;")]);
  end;

  "test_gen_client_method" >:: begin fun() ->
    assert_equal
      [ (0, "public void fun() {");
        (1,   "try {");	
        (2,     "iface.fun(this.name);");
        (1,   "} catch(RemoteError remoteError) {");
        (2,     "throw super.translateError(remoteError);");
        (1,   "}");
        (0, "}"); ]
      (gen_client_method { method_return_type = None;
                           method_name = "fun";
                           method_arguments = [];
                           method_decorators = []; });
    assert_equal
      [ (0, "public String fun() {");
        (1,   "try {");
        (2,     "return iface.fun(this.name);");
        (1,   "} catch(RemoteError remoteError) {");
        (2,     "throw super.translateError(remoteError);");
        (1,   "}");
        (0, "}"); ]
      (gen_client_method { method_return_type = Some String;
                           method_name = "fun";
                           method_arguments = [];
                           method_decorators = []; });
  end;

  "test_gen_interface" >:: begin fun() ->
    assert_equal
      "String f(String name, int x, String s);"
      (gen_interface { method_return_type = Some String;
                       method_name = "f";
                       method_arguments = [
                         { field_number = 1;
                           field_type = Int(true, 4);
                           field_name = "x"; };
                         { field_number = 2;
                           field_type = String;
                           field_name = "s"; };
                       ];
                       method_decorators = []
                     });

    assert_equal
      "void f(String name);"
      (gen_interface { method_return_type = None;
                       method_name = "f";
                       method_arguments = [];
                       method_decorators = []
                     });
  end;

  "test_gen_message_field"  >:: begin fun() ->
    assert_equal
      (0, "public String name = \"\";")
      (gen_message_field { field_number = 1;
                           field_type = String;
                           field_name = "name"; })
  end;

  "test_gen_to_string" >:: begin fun() ->
    assert_equal
      [ (0, "public String toString() {");
        (1,   "MessageStringGenerator gen = new MessageStringGenerator();");
        (1,   "gen.open(\"m\");");
        (1,   "gen.add(\"t\", t);");
        (1,   "gen.close();");
        (1,   "return gen.toString();";);
        (0, "}");
      ]
      (gen_to_string { message_name = "m";
                       message_fields = [
                         { field_name = "t";
                           field_type = String;
                           field_number = 1;
                         }
                       ];
                       message_raw = None })
  end;

] end

