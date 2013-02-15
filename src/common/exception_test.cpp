// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <string>
#include <gtest/gtest.h>
#include <pficommon/lang/demangle.h>
#include "exception.hpp"

using std::string;
using jubatus::exception::jubatus_exception;
using jubatus::exception::error_at_file;
using jubatus::exception::error_at_func;
using jubatus::exception::error_at_line;
using jubatus::exception::error_errno;
using jubatus::exception::error_file_name;
using jubatus::exception::error_api_func;
using jubatus::exception::error_message;
using jubatus::exception::error_info_list_t;
using jubatus::exception::exception_thrower_ptr;
using jubatus::exception::jubaexception;

namespace jubatus {
namespace exception {
typedef error_info<struct test_my_tag_, int> test_my_tag;
inline string to_string(const test_my_tag& info) {
  return pfi::lang::lexical_cast<string>(info.value() * 2);
}
}  // namespace exception
}  // namespace jubatus

TEST(error_info, defined_tag) {
  jubatus::exception::test_my_tag tag(1);

  EXPECT_FALSE(tag.splitter());
  EXPECT_EQ(1, tag.value());
}

TEST(error_info, error_info_base) {
  jubatus::exception::test_my_tag tag(1);
  const jubatus::exception::error_info_base& base = tag;

  EXPECT_FALSE(base.splitter());
  EXPECT_EQ("2", base.as_string());
}

namespace test_exception {
class ore_exception : public jubaexception<ore_exception> {
 public:
  explicit ore_exception(const std::string& what)
      : what_(what) {
  }
  ~ore_exception() throw () {
  }

  const char* what() const throw () {
    return what_.c_str();
  }
 private:
  std::string what_;
};

class derived_exception : public jubatus::exception::runtime_error {
 public:
  explicit derived_exception(const std::string& what)
      : jubatus::exception::runtime_error(what) {
  }
  ~derived_exception() throw () {
  }

  // multi-derived exception cannot get as it thrower
  // because thrower() returns exception_thrower_impl<runtime_error>
};

}  // namespace test_exception

TEST(exception, custom_exception) {
  exception_thrower_ptr thrower;

  try {
    throw test_exception::ore_exception("hogehoge");
  } catch (const jubatus_exception& e) {
     thrower = e.thrower();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(), test_exception::ore_exception);
}

TEST(exception, derived_exception) {
  exception_thrower_ptr thrower;

  try {
    throw test_exception::derived_exception("hogehoge");
  } catch (const jubatus_exception& e) {
     thrower = e.thrower();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(), jubatus::exception::runtime_error);
}

TEST(exception, derived_exception_dynamic_bind) {
  exception_thrower_ptr thrower;

  try {
    throw test_exception::derived_exception("hogehoge")
      << jubatus::exception::exception_thrower_binder_type();
  } catch (const jubatus_exception& e) {
     thrower = e.thrower();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(), test_exception::derived_exception);
}

TEST(exception, exception_with_error_info) {
  exception_thrower_ptr thrower;

  try {
    throw jubatus::exception::runtime_error("hogehoge")
      << error_at_file("exception_test.cpp")
      << error_at_line(12345)
      << error_at_func("custom_exception");
  } catch (const jubatus_exception& e) {
    error_info_list_t info_list = e.error_info();
    EXPECT_EQ(3u, info_list.size());
    EXPECT_EQ(string("exception_test.cpp"), info_list[0]->as_string());
    EXPECT_EQ(string("12345"), info_list[1]->as_string());
    EXPECT_EQ(string("custom_exception"), info_list[2]->as_string());
  }
}

TEST(exception, error_info) {
  // pre-defined error_info
  jubatus::exception::runtime_error("hogehoge")
    << error_at_file("exception_test.cpp")
    << error_at_line(12345)
    << error_at_func("custom_exception")
    << error_errno(0)
    << error_file_name("file_open.txt")
    << error_api_func("read")
    << error_message("message");
}

TEST(exception, error_errno_msg) {
  error_errno e(0);
  ASSERT_NE(string::npos, e.as_string().find("(0)"));
  ASSERT_NE(string(""), e.as_string());
}

TEST(exception, exception_info_macro) {
  try {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("hogehoge"));
  } catch (const jubatus_exception& e) {
    error_info_list_t info_list = e.error_info();
    EXPECT_EQ(4u, info_list.size());
    EXPECT_FALSE(info_list[0]->splitter());
    EXPECT_FALSE(info_list[1]->splitter());
    EXPECT_FALSE(info_list[2]->splitter());
    EXPECT_TRUE(info_list[3]->splitter());
  }
}

TEST(exception, exception_info_macro_additional) {
  try {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("hogehoge")
      << jubatus::exception::error_message("message"));
  } catch (const jubatus_exception& e) {
    error_info_list_t info_list = e.error_info();
    EXPECT_EQ(5u, info_list.size());
    EXPECT_FALSE(info_list[0]->splitter());
    EXPECT_EQ(string("message"), info_list[0]->as_string());
    EXPECT_FALSE(info_list[1]->splitter());
    EXPECT_FALSE(info_list[2]->splitter());
    EXPECT_FALSE(info_list[3]->splitter());
    EXPECT_TRUE(info_list[4]->splitter());
  }
}

TEST(exception, exception_info_macro_thrower) {
  exception_thrower_ptr thrower;

  try {
    throw JUBATUS_EXCEPTION(test_exception::derived_exception("hogehoge"));
  } catch (const jubatus_exception& e) {
    thrower = e.thrower();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(), test_exception::derived_exception);
}

TEST(exception, exception_custom_error_info) {
  try {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("hogehoge"));
  } catch (const jubatus_exception& e) {
    error_info_list_t info_list = e.error_info();
    EXPECT_EQ(4u, info_list.size());
    EXPECT_FALSE(info_list[0]->splitter());
    EXPECT_FALSE(info_list[1]->splitter());
    EXPECT_FALSE(info_list[2]->splitter());
    EXPECT_TRUE(info_list[3]->splitter());
  }
}

TEST(exception, exception_info_add_macro) {
  bool caught = false;

  try {
    try {
      throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("hogehoge"));
  } catch (const jubatus_exception& e) {
      // push additional information
      e << error_message("added");
      // push some error_info and end-mark of catch(error_splitter)
      e << JUBATUS_CURRENT_ERROR_INFO();
      // rethrow exception
      throw;
    }
  } catch (const jubatus_exception& e) {
    caught = true;
    error_info_list_t info_list = e.error_info();
    EXPECT_EQ(9u, info_list.size());
    EXPECT_FALSE(info_list[0]->splitter());
    EXPECT_FALSE(info_list[1]->splitter());
    EXPECT_FALSE(info_list[2]->splitter());
    EXPECT_TRUE(info_list[3]->splitter());
    EXPECT_FALSE(info_list[4]->splitter());
    EXPECT_EQ(string("added"), info_list[4]->as_string());
    EXPECT_FALSE(info_list[5]->splitter());
    EXPECT_FALSE(info_list[6]->splitter());
    EXPECT_FALSE(info_list[7]->splitter());
    EXPECT_TRUE(info_list[8]->splitter());
  }

  EXPECT_TRUE(caught);
}

TEST(exception, exception_error_info) {
  bool caught = false;
  try {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("what"));
  } catch (const jubatus_exception& e) {
    caught = true;
    EXPECT_EQ(string("what"), e.what());
  }

  EXPECT_TRUE(caught);
}

TEST(exception, current_exception) {
  exception_thrower_ptr thrower;

  try {
    throw std::runtime_error("my_runtime_error_hoge");
  } catch (...) {
    thrower = jubatus::exception::get_current_exception();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(), std::runtime_error);
}

TEST(exception, catch_unknown_current_exception) {
  exception_thrower_ptr thrower;

  try {
    throw 100;
  } catch (...) {
    thrower = jubatus::exception::get_current_exception();
  }

  EXPECT_TRUE(thrower);
  EXPECT_THROW(thrower->throw_exception(),
      jubatus::exception::unknown_exception);
}

#if __GNUC__ >= 3
TEST(exception, abi_demangle) {
  jubatus::exception::runtime_error object("a");
  EXPECT_EQ(string("jubatus::exception::runtime_error"),
      pfi::lang::demangle(typeid(object).name()));
}

TEST(exception, exception_class_name) {
  jubatus::exception::runtime_error object("a");
  EXPECT_EQ(string("jubatus::exception::runtime_error"),
      static_cast<jubatus_exception*>(&object)->name());
}
#endif

