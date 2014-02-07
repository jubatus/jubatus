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
    : public ::testing::TestWithParam<shared_ptr<driver::clustering> > {
 protected:
  void SetUp() {
    clustering_ = GetParam();
  }
  void TearDown() {
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

vector<shared_ptr<driver::clustering> > create_clusterings() {
  vector<shared_ptr<driver::clustering> > method;

  vector<pair<string, string> > method_pattern;
  method_pattern.push_back(make_pair("simple", "kmeans"));
  method_pattern.push_back(make_pair("simple", "gmm"));
  method_pattern.push_back(make_pair("compressive_kmeans", "kmeans"));
  method_pattern.push_back(make_pair("compressive_gmm", "gmm"));

  for (size_t i = 0; i < method_pattern.size(); ++i) {
    clustering_config conf;  // TODO(kumagi): is default  enough?
    conf.compressor_method = method_pattern[i].first;
    method.push_back(
        shared_ptr<driver::clustering>(
            new driver::clustering(
                shared_ptr<core::clustering::clustering>(
                    new core::clustering::clustering("dummy",
                                                     method_pattern[i].second,
                                                     conf)),
                make_fv_converter())
        ));
  }
  return method;
}

INSTANTIATE_TEST_CASE_P(clustering_test_instance,
    clustering_test,
    testing::ValuesIn(create_clusterings()));

}  // namespace driver
}  // namespace core
}  // namespace jubatus
