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
#include <set>
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
#include "../clustering/types.hpp"

using std::vector;
using std::string;
using std::pair;
using std::set;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::fv_converter::datum;
using jubatus::core::clustering::no_cluster_exception;
using jubatus::core::clustering::clustering_method;
using jubatus::core::clustering::clustering_config;

namespace jubatus {
namespace core {
namespace driver {

class clustering_test
    : public ::testing::TestWithParam<pair<string, string> > {
 protected:
  void SetUp() {
    pair<string, string> param = GetParam();
    clustering_config conf;
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

TEST_P(clustering_test, save_load) {
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
}

TEST_P(clustering_test, no_cluster) {
  core::fv_converter::datum d;
  vector<datum> datums;
  datums.push_back(single_datum("a", 1));
  clustering_->push(datums);

  ASSERT_THROW(clustering_->get_nearest_members(d), no_cluster_exception);
  ASSERT_THROW(clustering_->get_nearest_center(d), no_cluster_exception);
  ASSERT_THROW(clustering_->get_core_members(), no_cluster_exception);
}

TEST_P(clustering_test, get_k_center) {
  jubatus::util::math::random::mtrand r;
  vector<datum> one;
  vector<datum> two;

  for (int j = 0; j < 100 ; ++j) {
    datum a, b;
    a.num_values_.push_back(make_pair("a", -10 + r.next_gaussian() * 20));
    a.num_values_.push_back(make_pair("b", -200 + r.next_gaussian() * 400));
    b.num_values_.push_back(make_pair("c", -50 + r.next_gaussian() * 100));
    b.num_values_.push_back(make_pair("d", -25 + r.next_gaussian() * 50));
    one.push_back(a);
    two.push_back(b);
  }
  clustering_->push(one);
  clustering_->push(two);

  clustering_->do_clustering();
  {
    vector<datum> result = clustering_->get_k_center();
    ASSERT_EQ(2, result.size());
    if (result[0].num_values_[0].first == "a"
        || result[0].num_values_[0].first == "b") {
      // result[0] is {"a":xx, "b":yy} cluster
      if (result[0].num_values_[0].first == "a") {
        ASSERT_EQ("b", result[0].num_values_[1].first);
      } else {
        ASSERT_EQ("a", result[0].num_values_[1].first);
      }
    } else {
      // result[1] is {"a":xx, "b":yy} cluster
      if (result[1].num_values_[0].first == "a") {
        ASSERT_EQ("b", result[1].num_values_[1].first);
      } else {
        ASSERT_EQ("a", result[1].num_values_[1].first);
      }
    }
  }
}
struct check_points {
  float a;
  float b;
  check_points(float a_, float b_)
    : a(a_), b(b_) {}
  bool operator<(const check_points& rhs) const {
    if (a < rhs.a) {
      return true;
    } else if (rhs.a < a) {
      return false;
    } else if (b < rhs.b) {
      return true;
    } else {
      return false;
    }
  }
};

struct check_point_compare {
  bool operator()(const check_points& lhs, const check_points& rhs) {
    return lhs < rhs;
  }
};


TEST_P(clustering_test, get_nearest_members) {
  jubatus::util::math::random::mtrand r;
  vector<datum> one;
  vector<datum> two;

  set<check_points, check_point_compare> points;

  for (int i = 0; i < 1000 ; ++i) {
    datum x, y;
    float a = 10 + r.next_gaussian() * 20;
    float b = 1000 + r.next_gaussian() * 400;
    points.insert(check_points(a, b));

    x.num_values_.push_back(make_pair("a", a));
    x.num_values_.push_back(make_pair("b", b));
    y.num_values_.push_back(make_pair("c", -500000 - r.next_gaussian() * 100));
    y.num_values_.push_back(make_pair("d", -10000 - r.next_gaussian() * 50));
    one.push_back(x);
    two.push_back(y);
  }
  clustering_->push(one);
  clustering_->push(two);

  clustering_->do_clustering();

  {
    vector<datum> result = clustering_->get_k_center();
    ASSERT_EQ(2u, result.size());
  }

  for (set<check_points>::const_iterator it = points.begin();
       it != points.end();
       ++it) {
    datum x;
    x.num_values_.push_back(make_pair("a", it->a));
    x.num_values_.push_back(make_pair("b", it->b));
    core::clustering::cluster_unit result =
        clustering_->get_nearest_members(x);

    ASSERT_LT(1, result.size());
    for (size_t i = 0; i < result.size(); ++i) {
      const vector<pair<string, double> >& near_points =
          result[i].second.num_values_;
      ASSERT_EQ(2u, near_points.size());
      ASSERT_EQ("a", near_points[0].first);
      ASSERT_EQ("b", near_points[1].first);
      ASSERT_NE(points.end(),
                points.find(check_points(near_points[0].second,
                                         near_points[1].second)));
    }
  }
}



TEST_P(clustering_test, get_nearest_center) {
  jubatus::util::math::random::mtrand r;
  vector<datum> one;
  vector<datum> two;

  for (int i = 0; i < 1000 ; ++i) {
    datum x, y;
    x.num_values_.push_back(make_pair("a", 10 + r.next_gaussian() * 20));
    x.num_values_.push_back(make_pair("b", 1000 + r.next_gaussian() * 400));
    y.num_values_.push_back(make_pair("c", -500000 - r.next_gaussian() * 100));
    y.num_values_.push_back(make_pair("d", -10000 - r.next_gaussian() * 50));
    one.push_back(x);
    two.push_back(y);
  }
  clustering_->push(one);
  clustering_->push(two);

  clustering_->do_clustering();

  {
    vector<datum> result = clustering_->get_k_center();
    ASSERT_EQ(2, result.size());
  }

  for (int i = 0; i < 100; ++i) {
    {
      datum x;
      x.num_values_.push_back(make_pair("a", 10 + r.next_gaussian() * 20));
      x.num_values_.push_back(make_pair("b", 1000 + r.next_gaussian() * 400));
      datum expect_near_x = clustering_->get_nearest_center(x);
      const vector<pair<string, double> >& result = expect_near_x.num_values_;
      for (size_t i = 0; i < result.size(); ++i) {
        // Check if the result is belonging to the same cluster.
        // Difference of the value and the mean of its distribution are expected
        // to be lesser than 2-sigma.
        if (result[i].first == "a") {
          const double diff = std::abs(result[i].second - 10);
          ASSERT_GT(20 * 2, diff);
        } else if (result[i].first == "b") {
          const double diff = std::abs(result[i].second - 1000);
          ASSERT_GT(400 * 2, diff);
        }
      }
    }
  }
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
