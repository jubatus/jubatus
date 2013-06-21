// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include "jubatus/core/fv_converter/exception.hpp"
#include "ux_splitter.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::word_splitter;
using core::fv_converter::converter_exception;

TEST(ux_splitter, split) {
  std::vector<std::string> ks;
  const char* keywords[] = {
    "ueno",
    "tokyo",
    "shinjuku",
    "ikebukuro"
  };
  std::string doc = "ueno tokyo shinjuku ikebukuro";

  for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
    ks.push_back(keywords[i]);
  }
  std::vector<std::pair<size_t, size_t> > bounds;
  {
    ux_splitter splitter(ks);
    splitter.split(doc, bounds);
    ASSERT_EQ(4u, bounds.size());
  }
  {
    std::vector<std::string> ks2;
    ks2.push_back("o");
    ux_splitter splitter(ks2);

    splitter.split(doc, bounds);
    ASSERT_EQ(4u, bounds.size());
  }
}

TEST(ux_splitter, create) {
  std::map<std::string, std::string> param;
  ASSERT_THROW(create(param), converter_exception);

  param["dict_path"] = "unknown_file_name";
  ASSERT_THROW(create(param), converter_exception);

  param["dict_path"] = "../../../../jubatus/core/fv_converter/test_input";
  ASSERT_THROW(create(param), converter_exception);

  param["dict_path"] = "../../../../jubatus/core/fv_converter/test_input/keywords";
  pfi::lang::scoped_ptr<word_splitter> s(create(param));

  std::string d("hoge fuga");
  std::vector<std::pair<size_t, size_t> > bs;
  s->split(d, bs);
  ASSERT_EQ(2u, bs.size());
  ASSERT_EQ(0u, bs[0].first);
  ASSERT_EQ(4u, bs[0].second);
  ASSERT_EQ(5u, bs[1].first);
  ASSERT_EQ(4u, bs[1].second);
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
