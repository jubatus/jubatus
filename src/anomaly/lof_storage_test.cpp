#include <cmath>
#include <sstream>
#include <gtest/gtest.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/math/random.h>
#include "../common/hash.hpp"
#include "../common/portable_mixer.hpp"

#include "../recommender/recommender_mock.hpp"
#include "../recommender/recommender_mock_util.hpp"
#include "lof_storage.hpp"

using namespace std;
using namespace pfi::data::string;
using pfi::data::unordered_map;
using namespace pfi::lang;
using namespace pfi::math::random;
using jubatus::recommender::make_sfv;
using jubatus::recommender::make_ids;

namespace jubatus {
namespace storage {

namespace {

lof_storage* make_storage(uint32_t k,
                          uint32_t ck,
                          recommender::recommender_base* nn_engine) {
  map<string, string> config;
  config["name"] = "mock";
  config["lof:neighbor_num"] = lexical_cast<string>(k);
  config["lof:reverse_nn_Num"] = lexical_cast<string>(ck);

  lof_storage* s = new lof_storage(config);
  s->set_nn_engine(nn_engine);

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

}

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
    rmock_->set_neighbor_relation(make_sfv("1:2"), make_ids("1:1 0:2 -1:3 10:8"));

    storage_->update_all();
  }

  recommender::recommender_mock* rmock_;
  scoped_ptr<lof_storage> storage_;
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


class lof_storage_mix_test
    : public ::testing::TestWithParam<pair<int, string> > {
protected:
  static map<string, string> parse_config(const string& config_str) {
    const vector<string> lines = split(config_str, '\n');

    map<string, string> config;
    for (size_t i = 0; i < lines.size(); ++i) {
      vector<string> cols = split(lines[i], '\t');
      config[cols[0]] = cols[1];
    }
    return config;
  }

  sfv_t generate_gaussian(const string& name, const sfv_t& mean, float deviation) {
    sfv_t sfv(mean);
    const uint64_t seed = hash_util::calc_string_hash(name);
    mtrand r(seed);

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

  void mix() {
    portable_mixer_.mix();

    string diff;
    single_storage_->get_diff(diff);
    single_storage_->set_mixed_and_clear_diff(diff);
  }

  virtual void SetUp() {
    const pair<int, string>& param = GetParam();
    const int num_models = param.first;
    const map<string, string> config = parse_config(param.second);

    storages_.resize(num_models);
    for (int i = 0; i < num_models; ++i) {
      storages_[i].reset(new lof_storage(config));
    }
    single_storage_.reset(new lof_storage(config));

    for (size_t i = 0; i < storages_.size(); ++i) {
      portable_mixer_.add(storages_[i].get());
    }
  }

  virtual void TearDown() {
    portable_mixer_.clear();
    single_storage_.reset();
  }

  vector<shared_ptr<lof_storage> > storages_;
  shared_ptr<lof_storage> single_storage_;
  portable_mixer<lof_storage> portable_mixer_;
};

TEST_P(lof_storage_mix_test, consistency) {
  static const size_t kNumSample = 40;
  static const size_t kNumQuery = 1;
  static const float kDeviation = 1;

  const sfv_t mu0 = make_dense_sfv("1 1");
  const sfv_t mu1 = make_dense_sfv("2 1");

  for (size_t i = 0; i < kNumSample; ++i) {
    update(lexical_cast<string>(i), mu0, kDeviation);
  }

  mix();  // mix the recommenders

  for (size_t i = 0; i < kNumSample; ++i) {
    update(lexical_cast<string>(i), mu0, kDeviation);
  }

  mix();  // mix the latest k-dists and lrds

  for (size_t i = 0; i < kNumQuery; ++i) {
    const sfv_t x = generate_gaussian("t" + lexical_cast<string>(i), mu1, 1);
    float expect_lrd, actual_lrd;
    unordered_map<string, float> expect_lrds, actual_lrds;

    expect_lrd = single_storage_->collect_lrds(x, expect_lrds);

    for (size_t j = 0; j < storages_.size(); ++j) {
      actual_lrd = storages_[j]->collect_lrds(x, actual_lrds);
      EXPECT_FLOAT_EQ(expect_lrd, actual_lrd);

      for (unordered_map<string, float>::const_iterator it = expect_lrds.begin();
           it != expect_lrds.end(); ++it) {
        EXPECT_TRUE(actual_lrds.count(it->first));
        EXPECT_FLOAT_EQ(it->second, actual_lrds[it->first]);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    lof_storage_mix_test_instance,
    lof_storage_mix_test,
    ::testing::Values(
        make_pair(3,
                  string("anomaly:name\tlof\n"
                         "lof:neighbor_num\t10\n"
                         "lof:reverse_nn_num\t30\n"
                         "name\teuclid_lsh\n"
                         "lsh_num\t16\n"
                         "table_num\t16\n"
                         "bin_width\t1\n"))));

}
}
