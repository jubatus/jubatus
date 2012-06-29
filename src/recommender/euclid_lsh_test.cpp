#include <map>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/math/random.h>
#include "euclid_lsh.hpp"
#include "../common/hash.hpp"
#include "../common/portable_mixer.hpp"
#include "../storage/lsh_index_storage.hpp"

using namespace std;
using namespace pfi::data::string;
using namespace pfi::lang;
using namespace pfi::math::random;
using namespace jubatus::storage;

namespace jubatus {
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

}

class euclid_lsh_mix_test
    : public ::testing::TestWithParam<pair<int, string> > {
 protected:
  static const uint32_t kSeed = 1340764259;  // It may be any FIXED value

  static map<string, string> parse_config(const string& config_str) {
    const vector<string> lines = split(config_str, '\n');

    map<string, string> config;
    for (size_t i = 0; i < lines.size(); ++i) {
      vector<string> cols = split(lines[i], '\t');
      config[cols[0]] = cols[1];
    }
    return config;
  }

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

    const pair<int, string>& param = GetParam();
    const int num_models = param.first;
    const map<string, string> config = parse_config(param.second);

    recoms_.resize(num_models);
    for (int i = 0; i < num_models; ++i) {
      recoms_[i].reset(new euclid_lsh(config));
    }
    single_recom_.reset(new euclid_lsh(config));

    for (int i = 0; i < num_models; ++i) {
      portable_mixer_.add(recoms_[i]->get_storage());
    }
  }

  vector<shared_ptr<euclid_lsh> > recoms_;
  shared_ptr<euclid_lsh> single_recom_;
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

INSTANTIATE_TEST_CASE_P(
    euclid_lsh_mix_test_instance,
    euclid_lsh_mix_test,
    ::testing::Values(
        make_pair(2,
                  string("name\teuclid_lsh\n"
                         "lsh_num\t16\n"
                         "table_num\t4\n"
                         "bin_width\t1\n"))));

}
}
