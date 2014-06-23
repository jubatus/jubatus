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

#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "jubatus/util/math/random.h"
#include "jubatus/util/lang/cast.h"
#include "jubatus/util/text/json.h"

#include "jubatus/core/framework/diffv.hpp"
#include "clustering_client.hpp"
#include "clustering_serv.hpp"
#include "../clustering/clustering_config.hpp"
#include "../common/jsonconfig.hpp"
#include "../fv_converter/converter_config.hpp"

#include "test_util.hpp"

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::ifstream;
using std::stringstream;
using std::isfinite;
using std::numeric_limits;

using jubatus::util::lang::lexical_cast;
using jubatus::util::math::random::mtrand;
using jubatus::datum;
using jubatus::clustering::clustering_config;
using jubatus::jsonconfig::config_cast_check;
using jubatus::client::clustering;

static const int PORT = 65435;
static const char NAME[] = "clustering_test";

namespace {

class clustering_test : public ::testing::Test {
  protected:
    pid_t child_;

    clustering_test() {
      string config_path =
          "./test_input/config.clustering.kmeans-compressive.json";
      child_ = fork_process("clustering", PORT, config_path);
    };
    virtual ~clustering_test() {
      kill_process(child_);
    };
    virtual void restart_process() {
      kill_process(this->child_);
      string config_path =
          "./test_input/config.clustering.kmeans-compressive.json";
      this->child_ = fork_process("clustering", PORT, config_path);
    };
};

datum get_point(string id, size_t D) {
  static mtrand r(777);
  datum ret;
  for (size_t i = 0; i < D; ++i) {
    ret.num_values.push_back(
        make_pair(lexical_cast<string, int>(i), r.next_gaussian(0, 10)));
  }
  ret.string_values.push_back(make_pair("id", id));
  return ret;
}
//
datum get_point(size_t D) {
  return get_point("test_id", D);
}

vector<datum> get_points(
    size_t N, size_t D, string prefix, map<string, datum>& store) {
  vector<datum> ret;
  for (size_t i = 0; i < N; ++i) {
    string id = prefix + lexical_cast<string, int>(i);
    datum d = get_point(id, D);
    ret.push_back(d);
    store[id] = d;
  }
  return ret;
}
string get_id(const datum& d) {
  string id = "";
  typedef vector<pair<string, string> >::const_iterator prop_iter;
  for (prop_iter props = d.string_values.begin();
      props != d.string_values.end(); ++props) {
    if (props->first == "id") {
      id = props->second;
    }
  }
  return id;
}

bool is_registered(const vector<pair<double, datum> >& test,
                   const map<string, datum>& target) {
  typedef vector<pair<double, datum> >::const_iterator iter;
  for (iter it = test.begin(); it != test.end(); ++it) {
    string id = get_id(it->second);
    if (id == "") {
      return false;
    } else if (target.find(id) == target.end()) {
      return false;
    }
    const datum& d = target.find(id)->second;
    if (it->second.num_values != d.num_values
        || it->second.string_values != d.string_values) {
      return false;
    }
  }
  return true;
}

bool is_registered(const vector<vector<pair<double, datum> > >& test,
                   const map<string, datum>& target) {
  typedef vector<vector<pair<double, datum> > >::const_iterator iter;
  for (iter it = test.begin(); it != test.end(); ++it) {
    if (!is_registered(*it, target)) {
      return false;
    }
  }
  return true;
}

size_t get_total_num(const vector<vector<pair<double, datum> > >& points) {
  typedef vector<vector<pair<double, datum> > >::const_iterator iter;
  size_t ret = 0;
  for (iter it = points.begin(); it != points.end(); ++it) {
    ret += it->size();
  }
  return ret;
}

struct clustering_serv_config {
  std::string method;
  jubatus::util::data::optional<jubatus::util::text::json::json> parameter;
  jubatus::util::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(method) & JUBA_MEMBER(parameter) & JUBA_MEMBER(converter);
  }
};

clustering_config get_config(const string& config_str) {
  typedef jubatus::jsonconfig::config jsconf;
  jsconf config_root(lexical_cast<jubatus::util::text::json::json>(config_str));
  clustering_serv_config conf =
      config_cast_check<clustering_serv_config>(config_root);
  jsconf param;
  if (conf.parameter) {
    param = jsconf(*conf.parameter);
  }
  clustering_config cluster_conf =
      config_cast_check<clustering_config>(param);
  return cluster_conf;
}

TEST_F(clustering_test, small) {
  jubatus::client::clustering c("localhost", PORT, 100);
  clustering_config config = get_config(c.get_config(NAME));

  const uint32_t N = config.bucket_size;
  const uint32_t D = 2;
  using std::cout;
  using std::endl;
  std::cout << "N : " << N << std::endl;

  map<string, datum> original_points;
  c.push(NAME, get_points(N, D, "test1_", original_points));

  ASSERT_EQ(c.get_k_center(NAME).size(), config.k);
  ASSERT_EQ(c.get_nearest_center(NAME, get_point(D)).num_values.size(), D);
  ASSERT_TRUE(is_registered(c.get_core_members(NAME), original_points));
  ASSERT_EQ(get_total_num(c.get_core_members(NAME)),
            config.compressed_bucket_size);
  ASSERT_TRUE(is_registered(
      c.get_nearest_members(NAME, get_point(D)), original_points));

  c.save(NAME, "test");
}

TEST_F(clustering_test, load) {
  jubatus::client::clustering c("localhost", PORT, 100);
  clustering_config config = get_config(c.get_config(NAME));
  ASSERT_EQ(get_total_num(c.get_core_members(NAME)), 0);
  c.load(NAME, "test");
  ASSERT_EQ(c.get_k_center(NAME).size(), config.k);
  ASSERT_EQ(get_total_num(c.get_core_members(NAME)),
            config.compressed_bucket_size);
}

}  // namespace jubatus
