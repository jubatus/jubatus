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

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "linear_function_mixer.hpp"
#include "../storage/storage_base.hpp"

using std::string;
using std::make_pair;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

class storage_mock_base : public storage::storage_base {
 public:
  void get(const std::string& feature, feature_val1_t& ret) const {
  }
  void get2(const std::string& feature, feature_val2_t& ret) const {
  }
  void get3(const std::string& feature, feature_val3_t& ret) const {
  }

  void inp(const common::sfv_t& sfv, map_feature_val1_t& ret) const {
  }

  void set(
      const std::string& feature,
      const std::string& klass,
      const val1_t& w) {
  }
  void set2(
      const std::string& feature,
      const std::string& klass,
      const val2_t& w) {
  }
  void set3(
      const std::string& feature,
      const std::string& klass,
      const val3_t& w) {
  }

  void get_status(std::map<std::string, std::string>&) const {
  }

  void pack(msgpack::packer<msgpack::sbuffer>& packer) const {
  }
  void unpack(msgpack::object o) {
  }

  void update(
      const std::string& feature,
      const std::string& inc_class,
      const std::string& dec_class,
      const val1_t& w) {
  }

  void bulk_update(
      const common::sfv_t& sfv,
      float step_width,
      const std::string& inc_class,
      const std::string& dec_class) {
  }

  virtual void get_diff(diff_t&) const = 0;
  void set_average_and_clear_diff(const features3_t&) {
  }

  void register_label(const std::string& label) {
  }

  storage::version get_version() const {
    return storage::version();
  }

  virtual void clear() {
  }

  vector<string> get_labels() const {
    return vector<string>();
  }

  bool set_label(const string& label) {
    return true;
  }

  std::string type() const {
    return "";
  }
};

class storage_mock_1 : public storage_mock_base {
 public:
  void get_diff(diff_t& v) const {
    feature_val3_t c;
    c.push_back(make_pair(string("l1"), val3_t(1.0, 2.0, 3.0)));
    v.diff.push_back(make_pair(string("f1"), c));
  }
};

}  // namespace storage

namespace driver {

TEST(linear_function_mixer, diff) {
  linear_function_mixer m;
  m.set_model(linear_function_mixer::model_ptr(new storage::storage_mock_1));

  diffv d = m.get_diff_impl();
  EXPECT_EQ(1, d.count);
  ASSERT_EQ(1u, d.v.diff.size());
  EXPECT_EQ("f1", d.v.diff[0].first);
  ASSERT_EQ(1u, d.v.diff[0].second.size());
  EXPECT_EQ("l1", d.v.diff[0].second[0].first);
  EXPECT_EQ(1.0, d.v.diff[0].second[0].second.v1);
  EXPECT_EQ(2.0, d.v.diff[0].second[0].second.v2);
  EXPECT_EQ(3.0, d.v.diff[0].second[0].second.v3);
}


diffv make_diff(float v1, float v2, float v3, uint64_t version, size_t count) {
  diffv diff;
  storage::feature_val3_t c;
  c.push_back(make_pair(string("l1"), storage::val3_t(v1, v2, v3)));
  diff.v.diff.push_back(make_pair(string("f1"), c));
  diff.v.expect_version.set_number_unsafe(version);
  diff.count = count;
  return diff;
}

TEST(linear_function_mixer, mix) {
  linear_function_mixer m;

  diffv d1 = make_diff(1, 2, 3, 1, 5);
  diffv d2 = make_diff(2, 3, 4, 1, 3);
  diffv d;
  m.mix_impl(d1, d2, d);
  EXPECT_EQ(8, d.count);
  ASSERT_EQ(1u, d.v.diff.size());
  EXPECT_EQ("f1", d.v.diff[0].first);
  ASSERT_EQ(1u, d.v.diff[0].second.size());
  EXPECT_EQ("l1", d.v.diff[0].second[0].first);
  // (1 * 5 + 2 * 3) / (5 + 3)
  EXPECT_EQ(11./8., d.v.diff[0].second[0].second.v1);
  // min(2, 3)
  EXPECT_EQ(2., d.v.diff[0].second[0].second.v2);
  // (3 * 5 + 4 * 3) / (5 + 3)
  EXPECT_EQ(27./8., d.v.diff[0].second[0].second.v3);
}

TEST(linear_function_mixer, different_version_check) {
  linear_function_mixer m;

  diffv d1 = make_diff(1, 2, 3, 2, 5);
  diffv d2 = make_diff(2, 3, 4, 1, 3);  // should not mix
  diffv d;
  m.mix_impl(d1, d2, d);
  EXPECT_EQ(5, d.count);
  ASSERT_EQ(1u, d.v.diff.size());
  EXPECT_EQ("f1", d.v.diff[0].first);
  ASSERT_EQ(1u, d.v.diff[0].second.size());
  EXPECT_EQ("l1", d.v.diff[0].second[0].first);
  // (1 * 5) / 5
  EXPECT_EQ(5./5., d.v.diff[0].second[0].second.v1);
  // 2
  EXPECT_EQ(2., d.v.diff[0].second[0].second.v2);
  // 3 * 5 / 5
  EXPECT_EQ(15./5., d.v.diff[0].second[0].second.v3);
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
