// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/math/random.h>
#include "../common/exception.hpp"
#include "../common/hash.hpp"
#include "../common/portable_mixer.hpp"  // TODO(kashihara): use linear_mixer
#include "../recommender/recommender_mock.hpp"
#include "../recommender/recommender_mock_util.hpp"
#include "lof_storage.hpp"

using jubatus::core::recommender::make_sfv;
using jubatus::core::recommender::make_ids;
using jubatus::core::storage::lof_storage;
using pfi::data::unordered_map;
using pfi::lang::lexical_cast;
using std::istringstream;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

namespace {

lof_storage* make_storage(
    uint32_t k,
    uint32_t ck,
    recommender::recommender_base* mock_nn_engine) {
  lof_storage::config config;
  config.nearest_neighbor_num = k;
  config.reverse_nearest_neighbor_num = ck;
  lof_storage* s = new lof_storage(config, mock_nn_engine);
  // s->set_nn_engine(mock_nn_engine);

  return s;
}

sfv_t make_dense_sfv(const string& s) {
  sfv_t sfv;
  istringstream iss(s);

  size_t i = 0;
  float x = 0;
  while (iss >> x) {
    sfv.push_back(make_pair(lexical_cast<string>(i++), x));
  }

  return sfv;
}

}  // namespace

TEST(lof_storage, name) {
  lof_storage s;
  EXPECT_EQ("lof_storage", s.name());
}

TEST(lof_storage, get_all_row_ids) {
  lof_storage s;
  s.update_row("r1", make_sfv("1:1"));
  s.update_row("r2", make_sfv("2:1"));
  s.update_row("r4", make_sfv("1:-1"));
  s.update_row("r5", make_sfv("2:-1"));
  s.update_row("r6", make_sfv("3:-1"));

  s.remove_row("r4");

  s.update_row("r3", make_sfv("3:1"));
  s.remove_row("r2");

  vector<string> ids;
  s.get_all_row_ids(ids);
  sort(ids.begin(), ids.end());

  vector<string> expect;
  expect.push_back("r1");
  expect.push_back("r3");
  expect.push_back("r5");
  expect.push_back("r6");

  EXPECT_EQ(expect, ids);
}

// One dimensional example (points = { -1, 0, 1, 10 }, k = 2)
class lof_storage_one_dimensional_test : public ::testing::Test {
 protected:
  virtual void SetUp() {
    rmock_ = new recommender::recommender_mock;
    storage_.reset(make_storage(2, 2, rmock_));

    storage_->update_row("-1", make_sfv("1:-1"));
    storage_->update_row("0", make_sfv("1:0"));
    storage_->update_row("1", make_sfv("1:1"));
    storage_->update_row("10", make_sfv("1:10"));

    rmock_->set_neighbor_relation(make_sfv("1:-1"), make_ids("0:1 1:2 10:11"));
    rmock_->set_neighbor_relation(make_sfv("1:0"), make_ids("-1:1 1:1 10:10"));
    rmock_->set_neighbor_relation(make_sfv("1:1"), make_ids("0:1 -1:2 10:9"));
    rmock_->set_neighbor_relation(make_sfv("1:10"), make_ids("1:9 0:10 -1:11"));
    rmock_->set_neighbor_relation(make_sfv("1:2"),
                                  make_ids("1:1 0:2 -1:3 10:8"));

    storage_->update_all();
  }

  recommender::recommender_mock* rmock_;
  pfi::lang::scoped_ptr<lof_storage> storage_;
};

TEST_F(lof_storage_one_dimensional_test, get_kdist) {
  EXPECT_FLOAT_EQ(2.f, storage_->get_kdist("-1"));
  EXPECT_FLOAT_EQ(1.f, storage_->get_kdist("0"));
  EXPECT_FLOAT_EQ(2.f, storage_->get_kdist("1"));
  EXPECT_FLOAT_EQ(10.f, storage_->get_kdist("10"));
}

TEST_F(lof_storage_one_dimensional_test, get_lrd) {
  EXPECT_FLOAT_EQ(2/3.f, storage_->get_lrd("-1"));
  EXPECT_FLOAT_EQ(1/2.f, storage_->get_lrd("0"));
  EXPECT_FLOAT_EQ(2/3.f, storage_->get_lrd("1"));
  EXPECT_FLOAT_EQ(2/19.f, storage_->get_lrd("10"));
}

TEST_F(lof_storage_one_dimensional_test, collect_lrds) {
  unordered_map<string, float> lrds;
  float lrd = storage_->collect_lrds(make_sfv("1:0"), lrds);
  EXPECT_FLOAT_EQ(1/2.f, lrd);

  EXPECT_EQ(2u, lrds.size());
  EXPECT_FLOAT_EQ(2/3.f, lrds["-1"]);
  EXPECT_FLOAT_EQ(2/3.f, lrds["1"]);
}

TEST_F(lof_storage_one_dimensional_test, collect_lrds_novel_input) {
  unordered_map<string, float> lrds;
  float lrd = storage_->collect_lrds(make_sfv("1:2"), lrds);
  EXPECT_FLOAT_EQ(1/2.f, lrd);

  EXPECT_EQ(2u, lrds.size());
  EXPECT_FLOAT_EQ(2/3.f, lrds["1"]);
  EXPECT_FLOAT_EQ(1/2.f, lrds["0"]);
}

class lof_storage_mix_test : public ::testing::TestWithParam<
    std::pair<int, lof_storage::config> > {
 protected:
  sfv_t generate_gaussian(const string& name, const sfv_t& mean,
                          float deviation) {
    sfv_t sfv(mean);
    const uint64_t seed = hash_util::calc_string_hash(name);
    pfi::math::random::mtrand r(seed);

    for (size_t i = 0; i < sfv.size(); ++i) {
      sfv[i].second += r.next_gaussian() * deviation;
    }

    return sfv;
  }

  void update(const string& name, const sfv_t& mean, float deviation) {
    const sfv_t x = generate_gaussian(name, mean, deviation);
    lof_storage* storage = portable_mixer_.get_hash(name);
    storage->update_row(name, x);

    single_storage_->update_row(name, x);
  }

  void remove(const string& name) {
    portable_mixer_.get_hash(name)->remove_row(name);
    single_storage_->remove_row(name);
  }

  void mix() {
    portable_mixer_.mix();

    string diff;
    single_storage_->get_diff(diff);
    single_storage_->set_mixed_and_clear_diff(diff);
  }

  virtual void SetUp() {
    const std::pair<int, lof_storage::config>& param = GetParam();
    const int num_models = param.first;
    const lof_storage::config& config = param.second;

    storages_.resize(num_models);
    for (int i = 0; i < num_models; ++i) {
      storages_[i].reset(
        new lof_storage(config, new recommender::recommender_mock));
    }
    single_storage_.reset(
      new lof_storage(config, new recommender::recommender_mock));

    for (size_t i = 0; i < storages_.size(); ++i) {
      portable_mixer_.add(storages_[i].get());
    }
  }

  virtual void TearDown() {
    portable_mixer_.clear();
    single_storage_.reset();
  }

  vector<pfi::lang::shared_ptr<lof_storage> > storages_;
  pfi::lang::shared_ptr<lof_storage> single_storage_;
  portable_mixer<lof_storage> portable_mixer_;
};

TEST_P(lof_storage_mix_test, consistency) {
  static const size_t num_sample = 100;
  static const size_t num_query = 10;
  static const float deviation = 2;

  const sfv_t mu0 = make_dense_sfv("1 1");
  const sfv_t mu1 = make_dense_sfv("2 1");

  for (size_t i = 0; i < num_sample; ++i) {
    update(lexical_cast<string>(i), mu0, deviation);
  }

  mix();  // mix the recommenders

  for (size_t i = 0; i < num_sample; ++i) {
    update(lexical_cast<string>(i), mu0, deviation);
  }

  mix();  // mix the latest k-dists and lrds

  for (size_t i = 0; i < num_query; ++i) {
    const sfv_t x = generate_gaussian("t" + lexical_cast<string>(i), mu1, 1);
    float expect_lrd, actual_lrd;
    unordered_map<string, float> expect_lrds, actual_lrds;

    expect_lrd = single_storage_->collect_lrds(x, expect_lrds);

    for (size_t j = 0; j < storages_.size(); ++j) {
      actual_lrd = storages_[j]->collect_lrds(x, actual_lrds);
      EXPECT_FLOAT_EQ(expect_lrd, actual_lrd);

      for (unordered_map<string, float>::const_iterator it =
             expect_lrds.begin(); it != expect_lrds.end(); ++it) {
        EXPECT_TRUE(actual_lrds.count(it->first));
        EXPECT_FLOAT_EQ(it->second, actual_lrds[it->first]);
      }
    }
  }
}

TEST_P(lof_storage_mix_test, mix_after_remove) {
  static const size_t num_sample = 100;
  static const float deviation = 2;

  const sfv_t mu0 = make_dense_sfv("1 1");
  const sfv_t mu1 = make_dense_sfv("2 1");

  for (size_t i = 0; i < num_sample; ++i) {
    update(lexical_cast<string>(i), mu0, deviation);
  }
  mix();

  for (size_t i = 0; i < num_sample; ++i) {
    if (i % 2 == 0) {
      remove(lexical_cast<string>(i));
    }
  }
  mix();

  for (size_t i = 0; i < num_sample; ++i) {
    const string row = lexical_cast<string>(i);
    for (size_t j = 0; j < storages_.size(); ++j) {
      if (i % 2 == 0) {
        EXPECT_THROW(storages_[j]->get_kdist(row), exception::runtime_error);
        EXPECT_THROW(storages_[j]->get_lrd(row), exception::runtime_error);
      } else {
        EXPECT_NO_THROW(storages_[j]->get_kdist(row));
        EXPECT_NO_THROW(storages_[j]->get_lrd(row));
      }
    }
  }
}

lof_storage::config make_lof_storage_config() {
  lof_storage::config config;
  config.nearest_neighbor_num = 10;
  config.reverse_nearest_neighbor_num = 30;
  return config;
}

INSTANTIATE_TEST_CASE_P(
    lof_storage_mix_test_instance,
    lof_storage_mix_test,
    ::testing::Values(std::make_pair(5, make_lof_storage_config())));
}  // namespace storage
}  // namespcae core
}  // namespcae jubatus
