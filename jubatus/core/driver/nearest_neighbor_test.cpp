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

#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "jubatus/core/fv_converter/datum.hpp"

#include "../nearest_neighbor/lsh.hpp"
#include "nearest_neighbor.hpp"
#include "test_util.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

class nearest_neighbor_test : public ::testing::Test {
 protected:
  void SetUp() {
    shared_ptr<core::table::column_table> table(new core::table::column_table);
    core::nearest_neighbor::lsh::config conf;

    nearest_neighbor_.reset(new driver::nearest_neighbor(
        shared_ptr<core::nearest_neighbor::nearest_neighbor_base>(
            new core::nearest_neighbor::lsh(conf, table, "")),
        make_fv_converter()));
  }

  void TearDown() {
    nearest_neighbor_.reset();
  }

  shared_ptr<core::driver::nearest_neighbor> nearest_neighbor_;
};

TEST_F(nearest_neighbor_test, save_load) {
  {
    core::fv_converter::datum d;
    d.string_values_.push_back(std::make_pair("k1", "val"));
    nearest_neighbor_->set_row("1", d);
  }

  // save to a buffer
  msgpack::sbuffer sbuf;
  msgpack::packer<msgpack::sbuffer> packer(sbuf);
  nearest_neighbor_->get_mixable_holder()->pack(packer);

  // restart the driver
  TearDown();
  SetUp();

  // unpack the buffer
  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, sbuf.data(), sbuf.size());
  nearest_neighbor_->get_mixable_holder()->unpack(unpacked.get());

  std::vector<std::pair<std::string, float> > res
      = nearest_neighbor_->similar_row("1", 1);
  ASSERT_EQ(1u, res.size());
  EXPECT_EQ("1", res[0].first);
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
