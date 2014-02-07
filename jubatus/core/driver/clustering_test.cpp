// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <vector>
#include <string>
#include <utility>
#include <gtest/gtest.h>

#include "clustering.hpp"

#include "../../util/lang/shared_ptr.h"
#include "../clustering/clustering_config.hpp"
#include "../clustering/clustering.hpp"
#include "../clustering/kmeans_clustering_method.hpp"
#include "../clustering/gmm_clustering_method.hpp"
#include "test_util.hpp"
#include "../fv_converter/datum.hpp"

using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::fv_converter::datum;
using jubatus::core::clustering::clustering_method;
using jubatus::core::clustering::clustering_config;

namespace jubatus {
namespace core {
namespace driver {

class clustering_test
    : public ::testing::TestWithParam<pair<string, string> > {
 protected:
  void SetUp() {
    std::cout << GetParam().first << ": " << GetParam().second << std::endl;
    const pair<string, string> param = GetParam();
    clustering_config conf;  // TODO(kumagi): is default enough?
    conf.compressor_method = param.first;
    clustering_.reset(
        new driver::clustering(
            shared_ptr<core::clustering::clustering>(
                new core::clustering::clustering("dummy",
                                                 param.second,
                                                 conf)),
            make_fv_converter()));
  }
  void TearDown() {
    clustering_.reset();
  }
  shared_ptr<driver::clustering> clustering_;
};

namespace {  // testing util
datum single_datum(string key, double v) {
  datum d;
  d.num_values_.push_back(make_pair(key, v));
  return d;
}
}

TEST_P(clustering_test, get_revision) {
  for (int i = 0; i < 1000; ++i) {
    vector<datum> datums;
    datums.push_back(single_datum("a", 1));
    clustering_->push(datums);
  }
  ASSERT_EQ(0, clustering_->get_revision());
}

TEST_P(clustering_test, push) {
  for (int j = 0; j < 21 ; j += 5) {
    vector<datum> datums;
    for (int i = 0; i < 100; i += 5) {
      datums.push_back(single_datum("a", i * 2));
      datums.push_back(single_datum("b", i * 100));
      clustering_->push(datums);
    }
  }
}

TEST_F(clustering_test, save_load) {
  {
    core::fv_converter::datum d;
    vector<datum> datums;
    datums.push_back(single_datum("a", 1));
    clustering_->push(datums);
  }

  // save to a buffer
  msgpack::sbuffer sbuf;
  msgpack::packer<msgpack::sbuffer> packer(sbuf);
  clustering_->get_mixable_holder()->pack(packer);

  // restart the driver
  TearDown();
  SetUp();

  // unpack the buffer
  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, sbuf.data(), sbuf.size());
  clustering_->get_mixable_holder()->unpack(unpacked.get());

  // TODO(kumagi): it should check whether unpacked model is valid
}

vector<pair<string, string> > parameter_list() {
  vector<pair<string, string> > ret;
  ret.push_back(make_pair("simple", "kmeans"));
  ret.push_back(make_pair("compressive_kmeans", "kmeans"));
#ifdef JUBATUS_USE_EIGEN
  ret.push_back(make_pair("simple", "gmm"));
  ret.push_back(make_pair("compressive_gmm", "gmm"));
#endif
  return ret;
}

INSTANTIATE_TEST_CASE_P(clustering_test_instance,
    clustering_test,
    testing::ValuesIn(parameter_list()));

}  // namespace driver
}  // namespace core
}  // namespace jubatus
