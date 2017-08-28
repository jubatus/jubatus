// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2017 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <string>
#include <gtest/gtest.h>

#include "jubatus/core/fv_converter/exception.hpp"

#include "python_bridge.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

using std::map;
using std::string;
using jubatus::core::fv_converter::converter_exception;

TEST(python_bridge, init_setup) {
  // No errors are expected in intialize.
  initialize();

  // This is just for testing.
  add_path(".");

  // Setup parameters.
  map<string, string> params;

  // Missing module parameter.
  ASSERT_THROW(setup(params), converter_exception);

  // Missing class parameter.
  params["module"] = "no_such_module";
  ASSERT_THROW(setup(params), converter_exception);

  // Non-existent module.
  params["module"] = "no_such_module";
  params["class"] = "NoSuchClass";
  ASSERT_THROW(setup(params), converter_exception);

  // Non-existent class.
  params["module"] = "test_input.test_module";
  params["class"] = "NoSuchClass";
  ASSERT_THROW(setup(params), converter_exception);

  // Module load (success)
  params["module"] = "test_input.test_module";
  params["class"] = "Base";
  pb_object instance1(setup(params));
  EXPECT_NE(NULL, instance1);
}

TEST(python_bridge, pb_check) {
  scoped_gil lk;

  ASSERT_NO_THROW(PB_CHECK(true, "a"));
  ASSERT_THROW(PB_CHECK(false, "a"), converter_exception);
}

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
