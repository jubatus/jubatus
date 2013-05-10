open OUnit

let test_take_base _ =
  assert_equal "hoge" (File_util.take_base "/path/to/hoge.exe");
  assert_equal "hoge" (File_util.take_base "/path/to/hoge");
  assert_equal "hoge" (File_util.take_base "hoge");
;;

let test_split_path _ =
  assert_equal ["/"; "path"; "to"; "hoge"] (File_util.split_path "/path/to/hoge");
  assert_equal ["."; "path"; "to"; "hoge"] (File_util.split_path "path/to/hoge");
  assert_equal ["."; "path"; "to"; "hoge"] (File_util.split_path "./path/to/hoge");
  assert_equal ["/"] (File_util.split_path "/");
  assert_equal ["."] (File_util.split_path ".");
;;

let suite = "cpp.ml" >:::
  [ "test_take_base" >:: test_take_base;
    "test_split_path" >:: test_split_path;
  ]
;;
 
let _ =
  run_test_tt_main suite

