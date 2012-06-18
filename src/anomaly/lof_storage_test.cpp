#include <sstream>
#include <gtest/gtest.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/scoped_ptr.h>
#include "../recommender/recommender_mock.hpp"
#include "../recommender/recommender_mock_util.hpp"
#include "lof_storage.hpp"

using namespace std;
using pfi::data::unordered_map;
using pfi::lang::lexical_cast;
using pfi::lang::scoped_ptr;
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

    rmock_->set_neighbor_relation(make_sfv("1:-1"), make_ids("0:1 1:2 10:11"));
    rmock_->set_neighbor_relation(make_sfv("1:0"), make_ids("-1:1 1:1 10:10"));
    rmock_->set_neighbor_relation(make_sfv("1:1"), make_ids("0:1 -1:2 10:9"));
    rmock_->set_neighbor_relation(make_sfv("1:10"), make_ids("1:9 0:10 -1:11"));
    rmock_->set_neighbor_relation(make_sfv("1:2"), make_ids("1:1 0:2 -1:3 10:8"));

    storage_->update_row("-1", make_sfv("1:-1"));
    storage_->update_row("0", make_sfv("1:0"));
    storage_->update_row("1", make_sfv("1:1"));
    storage_->update_row("10", make_sfv("1:10"));

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

}
}
