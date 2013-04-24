// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <map>
#include <string>
#include <gtest/gtest.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include "local_storage.hpp"
#include "local_storage_mixture.hpp"

using std::istream;
using std::make_pair;
using std::map;
using std::ofstream;
using std::sort;
using std::stringstream;
using std::string;
using jubatus::key_manager;
using jubatus::sfv_t;
using jubatus::core::storage::feature_val1_t;
using jubatus::core::storage::feature_val2_t;
using jubatus::core::storage::feature_val3_t;
using jubatus::core::storage::map_feature_val1_t;
using jubatus::core::storage::val1_t;
using jubatus::core::storage::val2_t;
using jubatus::core::storage::val3_t;
using jubatus::core::storage::local_storage;
using jubatus::core::storage::local_storage_mixture;
using pfi::data::serialization::binary_iarchive;
using pfi::data::serialization::binary_oarchive;

namespace jubatus {
namespace core {
namespace storage {

class stub_storage : public storage_base {
 private:
  map<string, map<string, val3_t> > data_;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(data_);
  }

 public:
  void get_status(std::map<std::string, std::string>&) {
  }

  void get(const std::string& feature, feature_val1_t& ret) {
    const map<string, val3_t>& f = data_[feature];
    for (map<string, val3_t>::const_iterator it = f.begin(); it != f.end();
        ++it) {
      ret.push_back(make_pair(it->first, val1_t(it->second.v1)));
    }
  }

  void get2(const std::string& feature, feature_val2_t& ret) {
    const map<string, val3_t>& f = data_[feature];
    for (map<string, val3_t>::const_iterator it = f.begin(); it != f.end();
        ++it) {
      ret.push_back(make_pair(it->first, val2_t(it->second.v1, it->second.v2)));
    }
  }

  void get3(const std::string& feature, feature_val3_t& ret) {
    const map<string, val3_t>& f = data_[feature];
    for (map<string, val3_t>::const_iterator it = f.begin(); it != f.end();
        ++it) {
      ret.push_back(*it);
    }
  }

  void set(
      const std::string& feature,
      const std::string& klass,
      const val1_t& w) {
    data_[feature][klass] = val3_t(w, 0, 0);
  }

  void set2(
      const std::string& feature,
      const std::string& klass,
      const val2_t& w) {
    data_[feature][klass] = val3_t(w.v1, w.v2, 0);
  }

  void set3(
      const std::string& feature,
      const std::string& klass,
      const val3_t& w) {
    data_[feature][klass] = w;
  }

  bool save(std::ostream& os) {
    pfi::data::serialization::binary_oarchive oa(os);
    oa << *this;
    return true;
  }

  bool load(std::istream& is) {
    pfi::data::serialization::binary_iarchive ia(is);
    ia >> *this;
    return true;
  }

  void clear() {
    data_.clear();
  }

  std::string type() const {
    return "stub_storage";
  }
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

TEST(key_manager, trivial) {
  key_manager km;
  ASSERT_EQ(0u, km.get_id("x"));
  ASSERT_EQ(1u, km.get_id("y"));
  ASSERT_EQ(2u, km.get_id("z"));
  const char* tmp_file_name = "./tmp_key_manager";
  ofstream ofs(tmp_file_name);
  binary_oarchive oa(ofs);
  oa << km;
}

template <typename T>
class storage_test : public testing::Test {
};

TYPED_TEST_CASE_P(storage_test);

TYPED_TEST_P(storage_test, val1d) {
  TypeParam s;

  s.set("a", "x", 1);
  s.set("a", "y", 2);
  s.set("a", "z", 3);
  s.set("b", "x", 123);
  s.set("b", "z", 456);

  {
    feature_val1_t mm;
    s.get("a", mm);

    sort(mm.begin(), mm.end());

    feature_val1_t exp;
    exp.push_back(make_pair("x", val1_t(1)));
    exp.push_back(make_pair("y", val1_t(2)));
    exp.push_back(make_pair("z", val1_t(3)));

    EXPECT_TRUE(exp == mm);
  }

  {
    feature_val1_t mm;
    s.get("b", mm);

    sort(mm.begin(), mm.end());

    feature_val1_t exp;
    exp.push_back(make_pair("x", val1_t(123)));
    exp.push_back(make_pair("z", val1_t(456)));

    EXPECT_TRUE(exp == mm);
  }
}

TYPED_TEST_P(storage_test, val2d) {
  TypeParam s;

  s.set2("a", "x", val2_t(1, 11));
  s.set2("a", "y", val2_t(2, 22));
  s.set2("a", "z", val2_t(3, 33));
  s.set2("b", "x", val2_t(123, 123123));
  s.set2("b", "z", val2_t(456, 456456));

  {
    feature_val2_t mm;
    s.get2("a", mm);

    sort(mm.begin(), mm.end());

    feature_val2_t exp;
    exp.push_back(make_pair("x", val2_t(1, 11)));
    exp.push_back(make_pair("y", val2_t(2, 22)));
    exp.push_back(make_pair("z", val2_t(3, 33)));

    EXPECT_TRUE(exp == mm);
  }

  {
    feature_val2_t mm;
    s.get2("b", mm);

    sort(mm.begin(), mm.end());

    feature_val2_t exp;
    exp.push_back(make_pair("x", val2_t(123, 123123)));
    exp.push_back(make_pair("z", val2_t(456, 456456)));

    EXPECT_TRUE(exp == mm);
  }
}

TYPED_TEST_P(storage_test, val3d) {
  TypeParam s;

  s.set3("a", "x", val3_t(1, 11, 111));
  s.set3("a", "y", val3_t(2, 22, 222));
  s.set3("a", "z", val3_t(3, 33, 333));
  s.set3("b", "x", val3_t(12, 1212, 121212));
  s.set3("b", "z", val3_t(45, 4545, 454545));

  {
    feature_val3_t mm;
    s.get3("a", mm);

    sort(mm.begin(), mm.end());

    feature_val3_t exp;
    exp.push_back(make_pair("x", val3_t(1, 11, 111)));
    exp.push_back(make_pair("y", val3_t(2, 22, 222)));
    exp.push_back(make_pair("z", val3_t(3, 33, 333)));

    EXPECT_TRUE(exp == mm);
  }

  {
    feature_val3_t mm;
    s.get3("b", mm);

    sort(mm.begin(), mm.end());

    feature_val3_t exp;
    exp.push_back(make_pair("x", val3_t(12, 1212, 121212)));
    exp.push_back(make_pair("z", val3_t(45, 4545, 454545)));

    EXPECT_TRUE(exp == mm);
  }
}

TYPED_TEST_P(storage_test, serialize) {
  // const char* tmp_file_name = "./tmp_local_storage";

  stringstream ss;
  {
    TypeParam s;
    s.set3("a", "x", val3_t(1, 11, 111));
    s.set3("a", "y", val3_t(2, 22, 222));
    s.set3("a", "z", val3_t(3, 33, 333));
    s.set3("b", "x", val3_t(12, 1212, 121212));
    s.set3("b", "z", val3_t(45, 4545, 454545));

    // ofstream ofs(tmp_file_name);
    binary_oarchive oa(ss);
    oa << s;
  }

  {
    TypeParam s;
    // ifstream ifs(tmp_file_name);
    binary_iarchive ia(ss);
    ia >> s;
    // unlink(tmp_file_name);

    {
      feature_val3_t mm;
      s.get3("a", mm);

      sort(mm.begin(), mm.end());

      feature_val3_t exp;
      exp.push_back(make_pair("x", val3_t(1, 11, 111)));
      exp.push_back(make_pair("y", val3_t(2, 22, 222)));
      exp.push_back(make_pair("z", val3_t(3, 33, 333)));
      EXPECT_TRUE(exp == mm);
    }

    {
      feature_val3_t mm;
      s.get3("b", mm);

      sort(mm.begin(), mm.end());

      feature_val3_t exp;
      exp.push_back(make_pair("x", val3_t(12, 1212, 121212)));
      exp.push_back(make_pair("z", val3_t(45, 4545, 454545)));

      EXPECT_TRUE(exp == mm);
    }
  }
}

TYPED_TEST_P(storage_test, inp) {
  TypeParam s;
  sfv_t fv;
  map_feature_val1_t ret;
  s.inp(fv, ret);
  EXPECT_EQ(0u, ret.size());

  s.set3("f1", "class_x", val3_t(1, 11, 111));
  s.set3("f1", "class_y", val3_t(2, 22, 222));
  s.set3("f1", "class_z", val3_t(3, 33, 333));
  s.set3("f2", "class_x", val3_t(12, 1212, 121212));
  s.set3("f2", "class_z", val3_t(45, 4545, 454545));

  fv.push_back(make_pair("f2", 2.0));
  ret.clear();
  s.inp(fv, ret);

  EXPECT_EQ(2u, ret.size());
  ASSERT_LT(0u, ret.count("class_x"));
  ASSERT_EQ(0u, ret.count("class_y"));
  ASSERT_LT(0u, ret.count("class_z"));

  EXPECT_FLOAT_EQ(24.0, ret["class_x"]);
  EXPECT_FLOAT_EQ(90.0, ret["class_z"]);

  fv.push_back(make_pair("f1", 3.0));
  ret.clear();
  s.inp(fv, ret);

  EXPECT_EQ(3u, ret.size());
  ASSERT_LT(0u, ret.count("class_x"));
  ASSERT_LT(0u, ret.count("class_y"));
  ASSERT_LT(0u, ret.count("class_z"));

  EXPECT_FLOAT_EQ(27.0, ret["class_x"]);
  EXPECT_FLOAT_EQ(6.0, ret["class_y"]);
  EXPECT_FLOAT_EQ(99.0, ret["class_z"]);
}

template <typename T>
void get_expect_status(
    map<string, string>& before,
    map<string, string>& after) {
}

template<>
void get_expect_status<local_storage>(
    map<string, string>& before,
    map<string, string>& after) {
  before["num_features"] = "0";
  before["num_classes"] = "0";

  after["num_features"] = "2";
  after["num_classes"] = "3";
}

template<>
void get_expect_status<local_storage_mixture>(
    map<string, string>& before,
    map<string, string>& after) {
  before["num_features"] = "0";
  before["num_classes"] = "0";

  after["num_features"] = "2";
  after["num_classes"] = "3";
}

TYPED_TEST_P(storage_test, get_status) {
  TypeParam s;
  map<string, string> status;
  s.get_status(status);

  map<string, string> expect_before, expect_after;
  get_expect_status<TypeParam>(expect_before, expect_after);
  for (map<string, string>::const_iterator it = expect_before.begin();
      it != expect_before.end(); ++it) {
    ASSERT_LT(0U, status.count(it->first));
    EXPECT_EQ(it->second, status[it->first]);
  }

  s.set3("feature1", "class1", val3_t(1, 2, 3));
  s.set3("feature1", "class2", val3_t(1, 2, 3));
  s.set3("feature1", "class3", val3_t(1, 2, 3));

  s.set3("feature2", "class1", val3_t(1, 2, 3));
  s.set3("feature2", "class2", val3_t(1, 2, 3));

  status.clear();
  s.get_status(status);

  for (map<string, string>::const_iterator it = expect_after.begin();
      it != expect_after.end(); ++it) {
    ASSERT_LT(0u, status.count(it->first));
    EXPECT_EQ(it->second, status[it->first]);
  }
}

TYPED_TEST_P(storage_test, update) {
  TypeParam s;
  // add 1.0 to 'feature1' of 'class1',
  // and subtract 1.0 from 'feature1' of 'class2'
  s.update("feature1", "class1", "class2", val1_t(1.0));

  feature_val3_t v;
  s.get3("feature1", v);

  sort(v.begin(), v.end());

  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("class1", v[0].first);
  EXPECT_EQ(1.0, v[0].second.v1);
  EXPECT_EQ(0.0, v[0].second.v2);
  EXPECT_EQ(0.0, v[0].second.v3);

  EXPECT_EQ("class2", v[1].first);
  EXPECT_EQ(-1.0, v[1].second.v1);
  EXPECT_EQ(0.0, v[1].second.v2);
  EXPECT_EQ(0.0, v[1].second.v3);
}

TYPED_TEST_P(storage_test, bulk_update) {
  TypeParam s;

  sfv_t fv;
  fv.push_back(make_pair("feature1", 1.0));
  fv.push_back(make_pair("feature2", 2.0));

  s.bulk_update(fv, 1.5, "class1", "class2");

  feature_val3_t v;
  s.get3("feature1", v);
  sort(v.begin(), v.end());

  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("class1", v[0].first);
  EXPECT_EQ(1.5, v[0].second.v1);
  EXPECT_EQ(0.0, v[0].second.v2);
  EXPECT_EQ(0.0, v[0].second.v3);

  EXPECT_EQ("class2", v[1].first);
  EXPECT_EQ(-1.5, v[1].second.v1);
  EXPECT_EQ(0.0, v[1].second.v2);
  EXPECT_EQ(0.0, v[1].second.v3);

  v.clear();
  s.get3("feature2", v);
  sort(v.begin(), v.end());

  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("class1", v[0].first);
  EXPECT_EQ(3.0, v[0].second.v1);
  EXPECT_EQ(0.0, v[0].second.v2);
  EXPECT_EQ(0.0, v[0].second.v3);

  EXPECT_EQ("class2", v[1].first);
  EXPECT_EQ(-3.0, v[1].second.v1);
  EXPECT_EQ(0.0, v[1].second.v2);
  EXPECT_EQ(0.0, v[1].second.v3);
}

TYPED_TEST_P(storage_test, bulk_update_no_decrease) {
  TypeParam s;

  sfv_t fv;
  fv.push_back(make_pair("feature1", 1.0));
  fv.push_back(make_pair("feature2", 2.0));

  s.bulk_update(fv, 1.5, "class1", "");

  feature_val3_t v;
  s.get3("feature1", v);
  sort(v.begin(), v.end());

  ASSERT_EQ(1u, v.size());
  EXPECT_EQ("class1", v[0].first);
  EXPECT_EQ(1.5, v[0].second.v1);
  EXPECT_EQ(0.0, v[0].second.v2);
  EXPECT_EQ(0.0, v[0].second.v3);

  v.clear();
  s.get3("feature2", v);
  sort(v.begin(), v.end());

  ASSERT_EQ(1u, v.size());
  EXPECT_EQ("class1", v[0].first);
  EXPECT_EQ(3.0, v[0].second.v1);
  EXPECT_EQ(0.0, v[0].second.v2);
  EXPECT_EQ(0.0, v[0].second.v3);
}

TYPED_TEST_P(storage_test, clear) {
  TypeParam s;

  s.set3("a", "x", val3_t(1, 11, 111));
  s.set3("a", "y", val3_t(2, 22, 222));
  s.set3("a", "z", val3_t(3, 33, 333));
  s.set3("b", "x", val3_t(12, 1212, 121212));
  s.set3("b", "z", val3_t(45, 4545, 454545));

  {
    feature_val3_t mm;
    s.get3("a", mm);

    ASSERT_EQ(3u, mm.size());
  }

  {
    feature_val3_t mm;
    s.get3("b", mm);

    ASSERT_EQ(2u, mm.size());
  }

  s.clear();

  {
    feature_val3_t mm;
    s.get3("a", mm);

    ASSERT_EQ(0u, mm.size());
  }

  {
    feature_val3_t mm;
    s.get3("b", mm);

    ASSERT_EQ(0u, mm.size());
  }
}

REGISTER_TYPED_TEST_CASE_P(storage_test,
                           val1d,
                           val2d,
                           val3d,
                           serialize,
                           inp,
                           get_status,
                           update,
                           bulk_update,
                           bulk_update_no_decrease,
                           clear);

typedef testing::Types<
    jubatus::core::storage::stub_storage,
    local_storage,
    local_storage_mixture> storage_types;

INSTANTIATE_TYPED_TEST_CASE_P(st, storage_test, storage_types);
