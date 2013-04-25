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
#include "weight_manager.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(weight_manager, trivial) {
  weight_manager m;

  {
    sfv_t fv;
    m.add_weight("/address$tokyo@str", 1.5);
    m.update_weight(fv);
    m.get_weight(fv);

    fv.push_back(std::make_pair("/title$this@space#bin/bin", 1.0));
    // df = 1, |D| = 2
    fv.push_back(std::make_pair("/title$this@space#bin/idf", 1.0));
    fv.push_back(std::make_pair("/age@bin", 1.0));
    fv.push_back(std::make_pair("/address$tokyo@str#bin/weight", 1.0));
    m.update_weight(fv);
    m.get_weight(fv);

    ASSERT_EQ(4u, fv.size());
    EXPECT_FLOAT_EQ(1.0, fv[0].second);
    EXPECT_FLOAT_EQ(1.0 * log((2.0 + 1) / (1.0 + 1)), fv[1].second);
    EXPECT_FLOAT_EQ(1.0, fv[2].second);
    EXPECT_FLOAT_EQ(1.5, fv[3].second);
  }

  keyword_weights w = m.get_diff();
  EXPECT_EQ(2u, w.get_document_count());
  EXPECT_EQ(1u, w.get_document_frequency("/title$this@space#bin/idf"));
  EXPECT_EQ(1.5, w.get_user_weight("/address$tokyo@str"));

  {
    sfv_t fv;
    fv.push_back(std::make_pair("/title$this@space#bin/idf", 1.0));

    // df = 2, |D| = 3
    w.update_document_frequency(fv);
    w.add_weight("/title$hoge@str", 2.0);

    m.put_diff(w);

    EXPECT_EQ(0u, m.get_diff().get_document_count());

    // df = 3, |D| = 4
    m.update_weight(fv);
    m.get_weight(fv);
    EXPECT_EQ(1u, fv.size());
    EXPECT_FLOAT_EQ(1.0 * log((4.0 + 1) / (3.0 + 1)), fv[0].second);
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
