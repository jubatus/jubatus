C++ client test
===================

test\_gtest.py is a C++ Jubatus client test(gtest) runner for jubatest.
In envdef.py, specify jubatest.constants as a key and set path to value.
See envdef.sample.py for the example.

To run the tests:

```
$ ./waf configure

$ ./waf build --checkall
  (note: you can ignore the test errors here)

$ jubatest --config envdef.py --testcase . --pattern test_gtest.py
```
