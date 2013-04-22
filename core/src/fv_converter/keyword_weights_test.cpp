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

#include <cmath>
#include <utility>
#include <gtest/gtest.h>
#include "../common/type.hpp"
#include "keyword_weights.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(keyword_weights, trivial) {
  keyword_weights m, m2;
  {
    sfv_t fv;

    m.update_document_frequency(fv);

    fv.push_back(std::make_pair("key1", 1.0));
    fv.push_back(std::make_pair("key2", 1.0));
    m.update_document_frequency(fv);

    m.add_weight("key3", 2.0);

    EXPECT_EQ(2u, m.get_document_count());
    EXPECT_EQ(1u, m.get_document_frequency("key1"));
    EXPECT_EQ(0u, m.get_document_frequency("unknown"));
  }

  {
    sfv_t fv;
    m2.update_document_frequency(fv);

    fv.push_back(std::make_pair("key1", 1.0));
    fv.push_back(std::make_pair("key2", 1.0));
    m2.update_document_frequency(fv);

    m2.add_weight("key3", 3.0);

    m.merge(m2);

    EXPECT_EQ(4u, m.get_document_count());
    EXPECT_EQ(2u, m.get_document_frequency("key1"));
    EXPECT_EQ(3.0, m.get_user_weight("key3"));
    EXPECT_EQ(0u, m.get_document_frequency("unknown"));
  }

  {
    m.clear();
    EXPECT_EQ(0u, m.get_document_count());
    EXPECT_EQ(0u, m.get_document_frequency("key1"));
    EXPECT_EQ(0.0, m.get_user_weight("key3"));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
