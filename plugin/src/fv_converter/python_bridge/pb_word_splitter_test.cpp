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
#include <vector>
#include <utility>
#include <string>
#include <gtest/gtest.h>

#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/core/fv_converter/string_feature.hpp"

#include "python_bridge.hpp"
#include "pb_word_splitter.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

using std::map;
using std::vector;
using std::string;
using std::pair;

TEST(pb_word_splitter, trivial) {
  // Test fixture setup.
  initialize();
  add_path(".");

  std::map<string, string> params;
  params["module"] = "test_input.test_module";
  params["class"] = "WordSplitter";
  pb_word_splitter pb(setup(params));

  vector<pair<size_t, size_t> > result;
  pb.split("my_text", result);
  ASSERT_EQ(1, result.size());
  EXPECT_EQ(0, result[0].first);
  EXPECT_EQ(1, result[0].second);
}

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
