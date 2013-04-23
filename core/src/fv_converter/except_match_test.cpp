// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <gtest/gtest.h>

#include "except_match.hpp"
#include "match_all.hpp"
#include "exact_match.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

typedef pfi::lang::shared_ptr<key_matcher> matcher_ptr;

TEST(except_match, trivial) {
  except_match m(
      matcher_ptr(new match_all()),
      matcher_ptr(new exact_match("hoge")));
  ASSERT_FALSE(m.match("hoge"));
  ASSERT_TRUE(m.match("fuga"));
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
