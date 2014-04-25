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

#include "../anomaly/anomaly.hpp"
#include "../fv_converter/datum.hpp"
#include "../recommender/recommender.hpp"
#include "anomaly.hpp"
#include "test_util.hpp"

using std::make_pair;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

class anomaly_test : public ::testing::Test {
 protected:
  void SetUp() {
    core::anomaly::lof_storage::config lof_config;
    lof_config.nearest_neighbor_num = 100;
    lof_config.reverse_nearest_neighbor_num = 30;
    core::recommender::euclid_lsh::config lsh_config;
    lsh_config.hash_num = 8;
    lsh_config.table_num = 8;
    lsh_config.probe_num = 8;
    lsh_config.bin_width = 8.2;
    lsh_config.seed = 1234;

    anomaly_.reset(new driver::anomaly(
          shared_ptr<core::anomaly::anomaly_base>(
            new core::anomaly::lof(lof_config,
              shared_ptr<core::recommender::recommender_base>(
                new core::recommender::euclid_lsh(lsh_config)))),
          make_fv_converter()));
  }

  void TearDown() {
    anomaly_.reset();
  }

  jubatus::util::lang::shared_ptr<core::driver::anomaly> anomaly_;
};

TEST_F(anomaly_test, small) {
  {
    fv_converter::datum datum;
    datum.num_values_.push_back(make_pair("f1", 1.0));

    anomaly_->add("1", datum);  // is it good to be inf?
    std::pair<std::string, float> w = anomaly_->add("2", datum);
    float v = anomaly_->calc_score(datum);
    ASSERT_DOUBLE_EQ(w.second, v);
  }
  {
    std::vector<std::string> rows = anomaly_->get_all_rows();
    ASSERT_EQ(2u, rows.size());
  }


  // save
  msgpack::sbuffer sbuf;
  msgpack::packer<msgpack::sbuffer> pk(sbuf);
  anomaly_->pack(pk);

  // clear
  anomaly_->clear();
  {
    std::vector<std::string> rows = anomaly_->get_all_rows();
    ASSERT_EQ(0u, rows.size());
  }

  { // load
    msgpack::unpacked msg;
    msgpack::unpack(&msg, sbuf.data(), sbuf.size());
    anomaly_->unpack(msg.get());
    std::vector<std::string> rows = anomaly_->get_all_rows();
    ASSERT_EQ(2u, rows.size());
  }

}

}  // driver namespace
}  // core namespace
}  // jubatus namespace
