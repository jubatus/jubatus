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

#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/math/random.h>

#include "euclid_lsh.hpp"
#include "../common/hash.hpp"
#include "../common/portable_mixer.hpp"  // TODO(suma): use linear_mixer
#include "../storage/lsh_index_storage.hpp"

using std::istringstream;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;
using pfi::lang::lexical_cast;
using pfi::math::random::mtrand;
using jubatus::core::storage::lsh_index_storage;

namespace jubatus {
namespace core {
namespace recommender {

namespace {

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

class euclid_lsh_mix_test
    : public ::testing::TestWithParam<pair<int, euclid_lsh::config> > {
 protected:
  static const uint32_t kSeed = 1340764259;  // It may be any FIXED value

  sfv_t generate_gaussian(const sfv_t& mean, float deviation) {
    sfv_t sfv(mean);
    for (size_t i = 0; i < sfv.size(); ++i) {
      sfv[i].second += rand_.next_gaussian() * deviation;
    }
    return sfv;
  }

  void update(const string& name, const sfv_t& mean, float deviation) {
    const sfv_t x = generate_gaussian(mean, deviation);
    euclid_lsh* recom =
        recoms_[hash_util::calc_string_hash(name) % recoms_.size()].get();
    recom->update_row(name, x);

    single_recom_->update_row(name, x);
  }

  void mix() {
    portable_mixer_.mix();
  }

  virtual void SetUp() {
    rand_ = mtrand(kSeed);

    const pair<int, euclid_lsh::config>& param = GetParam();
    const int num_models = param.first;
    const euclid_lsh::config& config = param.second;

    recoms_.resize(num_models);
    for (int i = 0; i < num_models; ++i) {
      recoms_[i].reset(new euclid_lsh(config));
    }
    single_recom_.reset(new euclid_lsh(config));

    for (int i = 0; i < num_models; ++i) {
      portable_mixer_.add(recoms_[i]->get_storage());
    }
  }

  vector<pfi::lang::shared_ptr<euclid_lsh> > recoms_;
  pfi::lang::shared_ptr<euclid_lsh> single_recom_;
  portable_mixer<lsh_index_storage> portable_mixer_;

  mtrand rand_;
};

TEST_P(euclid_lsh_mix_test, consistency) {
  static const size_t kNumSample = 100;
  static const size_t kNumRetrieval = 10;
  static const size_t kNumQuery = 5;
  static const float kDeviation = 1;

  const sfv_t mu0 = make_dense_sfv("1 1");
  const sfv_t mu1 = make_dense_sfv("2 1");

  for (size_t i = 0; i < kNumSample; ++i) {
    update(lexical_cast<string>(i), mu0, kDeviation);
  }
  mix();

  for (size_t i = 0; i < kNumQuery; ++i) {
    const sfv_t x = generate_gaussian(mu1, 1);
    vector<pair<string, float> > expect, actual;

    single_recom_->neighbor_row(x, expect, kNumRetrieval);

    for (size_t j = 0; j < recoms_.size(); ++j) {
      recoms_[j]->neighbor_row(x, actual, kNumRetrieval);

      for (size_t k = 0; k < expect.size(); ++k) {
        EXPECT_EQ(expect[k].first, actual[k].first);
        EXPECT_FLOAT_EQ(expect[k].second, actual[k].second);
      }
    }
  }
}

euclid_lsh::config make_euclid_lsh_config() {
  euclid_lsh::config config;

  config.lsh_num = 16;
  config.table_num = 4;
  config.bin_width = 1;
  return config;
}

INSTANTIATE_TEST_CASE_P(
    euclid_lsh_mix_test_instance,
    euclid_lsh_mix_test,
    ::testing::Values(make_pair(2, make_euclid_lsh_config())));

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
