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

#include "jubatus/core/fv_converter/datum.hpp"
#include "../common/jsonconfig.hpp"

#include "../nearest_neighbor/nearest_neighbor_factory.hpp"
#include "nearest_neighbor.hpp"
#include "test_util.hpp"

using std::vector;
using std::pair;
using std::make_pair;
using std::string;
using jubatus::util::lang::lexical_cast;
using jubatus::util::lang::shared_ptr;
using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::json_integer;
using jubatus::util::text::json::json_string;
using jubatus::util::text::json::json_float;
using jubatus::core::fv_converter::datum;

namespace jubatus {
namespace core {
namespace driver {

class nearest_neighbor_test
    : public ::testing::TestWithParam<shared_ptr<driver::nearest_neighbor> > {
 protected:
  void SetUp() {
    nearest_neighbor_ = GetParam();
  }
  void TearDown() {
  }
  shared_ptr<core::driver::nearest_neighbor> nearest_neighbor_;
};

datum single_num_datum(const string& key, double value) {
  core::fv_converter::datum d;
  d.num_values_.push_back(make_pair(key, value));
  return d;
}
datum single_str_datum(const string& key, const string& value) {
  core::fv_converter::datum d;
  d.string_values_.push_back(make_pair(key, value));
  return d;
}

TEST_P(nearest_neighbor_test, set_row) {
  nearest_neighbor_->set_row("a", single_str_datum("a", "hoge"));
}

TEST_P(nearest_neighbor_test, similar_row_from_id) {
  nearest_neighbor_->set_row("a", single_str_datum("x", "hoge"));
  nearest_neighbor_->set_row("b", single_str_datum("y", "fuga"));
  vector<pair<string, float> > result =
      nearest_neighbor_->similar_row("a", 100);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0].first);
  ASSERT_EQ("b", result[1].first);
}

TEST_P(nearest_neighbor_test, similar_row_from_data) {
  nearest_neighbor_->set_row("a", single_str_datum("x", "hoge"));
  nearest_neighbor_->set_row("b", single_str_datum("y", "fuga"));
  vector<pair<string, float> > result =
      nearest_neighbor_->similar_row(single_str_datum("x", "hoge"), 100);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0].first);
  ASSERT_EQ("b", result[1].first);
}

TEST_P(nearest_neighbor_test, neighbor_row_from_id) {
  nearest_neighbor_->set_row("a", single_str_datum("x", "hoge"));
  nearest_neighbor_->set_row("b", single_str_datum("y", "fuga"));
  vector<pair<string, float> > result =
      nearest_neighbor_->neighbor_row_from_id("a", 100);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0].first);
  ASSERT_EQ("b", result[1].first);
}

TEST_P(nearest_neighbor_test, neighbor_row2_from_data) {
  nearest_neighbor_->set_row("a", single_str_datum("x", "hoge"));
  nearest_neighbor_->set_row("b", single_str_datum("y", "fuga"));
  vector<pair<string, float> > result =
      nearest_neighbor_->neighbor_row_from_data(
          single_str_datum("x", "hoge"), 100);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0].first);
  ASSERT_EQ("b", result[1].first);
}

TEST_P(nearest_neighbor_test, neighbor_row_and_similar_row) {
  // neighbor_row and similar_row should return the same order result
  const size_t num = 200;
  for (size_t i = 0; i < num; ++i) {
    const string key = lexical_cast<string>(i);
    datum d = single_str_datum("x" + key, "a");
    d.string_values_.push_back(make_pair("y", "b"));
    d.string_values_.push_back(make_pair("z" + key + key, "c"));
    nearest_neighbor_->set_row(key, d);
  }

  for (size_t i = 0; i < num; ++i) {
    const string key = lexical_cast<string>(i);
    datum d = single_str_datum("x" + key, "a");

    vector<pair<string, float> > nr_result =
        nearest_neighbor_->neighbor_row_from_data(d, 100);
    vector<pair<string, float> > sr_result =
        nearest_neighbor_->similar_row(d, 100);
    ASSERT_EQ(nr_result.size(), sr_result.size());
    for (size_t j = 0; j < nr_result.size(); ++j) {
      ASSERT_EQ(nr_result[j].first, sr_result[j].first);
    }
  }
}

TEST_P(nearest_neighbor_test, clear) {
  for (int i = 0; i < 100; ++i) {
    nearest_neighbor_->set_row("a" + lexical_cast<string>(i),
                               single_num_datum("x", i));
  }
  {
    vector<pair<string, float> > result =
        nearest_neighbor_->neighbor_row_from_data(
            single_num_datum("x", 1), 100);
    ASSERT_EQ("a1", result[0].first);
  }
  nearest_neighbor_->clear();
  {
    for (int i = 0; i < 100; ++i) {
    vector<pair<string, float> > result =
        nearest_neighbor_->neighbor_row_from_data(
          single_num_datum("a" + lexical_cast<string>(i), i), 100);
      ASSERT_EQ(0u, result.size());
    }
  }
}

TEST_P(nearest_neighbor_test, save_load) {
  {
    core::fv_converter::datum d;
    d.string_values_.push_back(make_pair("k1", "val"));
    nearest_neighbor_->set_row("1", d);
  }

  // save to a buffer
  msgpack::sbuffer sbuf;
  msgpack::packer<msgpack::sbuffer> packer(sbuf);
  nearest_neighbor_->get_mixable_holder()->pack(packer);

  // restart the driver
  TearDown();
  SetUp();

  // unpack the buffer
  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, sbuf.data(), sbuf.size());
  nearest_neighbor_->get_mixable_holder()->unpack(unpacked.get());

  vector<pair<string, float> > res
      = nearest_neighbor_->similar_row("1", 1);
  ASSERT_EQ(1u, res.size());
  EXPECT_EQ("1", res[0].first);
}

vector<shared_ptr<driver::nearest_neighbor> > create_nearest_neighbors() {
  vector<shared_ptr<driver::nearest_neighbor> > method;

  vector<pair<string, int> > pattern;
  for (size_t i = 8; i < 3000; i = i << 1) {  // up to 2048
    pattern.push_back(make_pair("lsh", i));
    pattern.push_back(make_pair("euclid_lsh", i));
    pattern.push_back(make_pair("minhash", i));
  }
  for (size_t i = 0; i < pattern.size(); ++i) {
    shared_ptr<core::table::column_table> table(new core::table::column_table);

    json jsconf(new json_object);
    jsconf["hash_num"] = new json_integer(pattern[i].second);
    common::jsonconfig::config conf(jsconf);
    method.push_back(
        shared_ptr<driver::nearest_neighbor>(
            new driver::nearest_neighbor(
                core::nearest_neighbor::create_nearest_neighbor(
                    pattern[i].first,
                    conf,
                    table,
                    ""),
                make_fv_converter())));
  }
  return method;
}

INSTANTIATE_TEST_CASE_P(nearest_neighbor_test_instance,
    nearest_neighbor_test,
    testing::ValuesIn(create_nearest_neighbors()));

TEST_P(nearest_neighbor_test, small) {
  nn_driver_->set_row("id1", create_datum_2d(2.f, 0.f));
  nn_driver_->set_row("id2", create_datum_2d(2.f, 1.f));
  nn_driver_->set_row("id3", create_datum_2d(0.f, 2.f));

  nn_driver_->neighbor_row_from_id("id1", 2);
  nn_driver_->neighbor_row_from_id("id2", 2);
  nn_driver_->neighbor_row_from_id("id3", 2);

  nn_driver_->neighbor_row_from_data(create_datum_2d(1.f, 1.f), 2);
}

INSTANTIATE_TEST_CASE_P(
    nearest_neighbor_test_instance, nearest_neighbor_test,
    testing::ValuesIn(create_nearest_neighbors()));

class nearest_neighbor_with_unlearning_test
    : public ::testing::TestWithParam<
          std::tr1::tuple<
              shared_ptr<nearest_neighbor_base>,
              shared_ptr<unlearner_base> > > {
 protected:
  void SetUp() {
    nn_driver_.reset(new nearest_neighbor(
        std::tr1::get<0>(GetParam()), make_fv_converter(),
        std::tr1::get<1>(GetParam())));
  }

  bool is_hit(
      const std::string& should_hit_id,
      const fv_converter::datum& d,
      size_t size) const {
    std::vector<std::pair<std::string, float> > hit =
        nn_driver_->neighbor_row_from_data(d, size);
    for (size_t i = 0; i < hit.size(); ++i) {
      if (hit[i].first == should_hit_id) {
        return true;
      }
    }
    return false;
  }

  shared_ptr<nearest_neighbor> nn_driver_;
};

TEST_P(nearest_neighbor_with_unlearning_test, unlearning) {
  nn_driver_->set_row("id1", create_datum_2d(2.f, 0.f));
  nn_driver_->set_row("id2", create_datum_2d(2.f, 1.f));
  nn_driver_->set_row("id3", create_datum_2d(0.f, 2.f));
  EXPECT_TRUE(is_hit("id1", create_datum_2d(1.f, 1.f), 3));

  nn_driver_->set_row("id2", create_datum_2d(2.f, 2.f));
  EXPECT_TRUE(is_hit("id1", create_datum_2d(1.f, 1.f), 3));

  nn_driver_->set_row("id4", create_datum_2d(1.f, 2.f));
  size_t hit_count = 0;
  hit_count += is_hit("id1", create_datum_2d(1.f, 1.f), 3);
  hit_count += is_hit("id2", create_datum_2d(1.f, 1.f), 3);
  hit_count += is_hit("id3", create_datum_2d(1.f, 1.f), 3);
  EXPECT_EQ(2u, hit_count);
}

INSTANTIATE_TEST_CASE_P(
    nearest_neighbor_with_unlearning_test_instance,
    nearest_neighbor_with_unlearning_test,
    ::testing::Combine(
        ::testing::ValuesIn(create_nearest_neighbors()),
        ::testing::ValuesIn(create_unlearners())));

}  // namespace driver
}  // namespace core
}  // namespace jubatus
