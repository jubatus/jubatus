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

#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <utility>

#include "recommender_factory.hpp"
#include "../common/jsonconfig.hpp"


using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::to_json;

typedef std::pair<
    std::string,jubatus::core::common::jsonconfig::config> recommender_parameter;

namespace std {
void PrintTo(const recommender_parameter& rec, ::std::ostream* os) {
  *os << "{ algorithm: " << rec.first << ", "
     << " parameter: ";
  rec.second.get().print(*os, false);
  *os << " }";
}
}  // namespace std
namespace jubatus {
namespace core {
namespace recommender {

class recommender_factory_test
  : public ::testing::TestWithParam<recommender_parameter> {
 protected:
  void SetUp() {
    name_ = GetParam().first;
    conf_ = GetParam().second;
  }

  std::string name_;
  common::jsonconfig::config conf_;
};

std::vector<recommender_parameter> generate_parameters() {
  std::vector<recommender_parameter> ret;

  {  // inverted index
    json js(new json_object);
    ret.push_back(
        recommender_parameter(
          "inverted_index",
          common::jsonconfig::config(js)));
    {  // unlearn
      // TODO(kumagi): is it valid?
      json js_unlearn(js.clone());
      js_unlearn["unlearner"] = to_json(std::string("lru"));
      js_unlearn["unlearner_parameter"] = new json_object;
      js_unlearn["unlearner_parameter"]["max_size"] = to_json(1);
      ret.push_back(
          recommender_parameter(
              "inverted_index",
              common::jsonconfig::config(js_unlearn)));
    }
  }

  {  // minhash / lsh
    json js(new json_object);
    json js_unlearn(js.clone());
    js["hash_num"] = to_json(64);
    common::jsonconfig::config conf(js);
    ret.push_back(make_pair("minhash",conf));
    ret.push_back(make_pair("lsh",conf));
  }

  {  // euclid_lsh
    json js(new json_object);
    js["hash_num"] = to_json(64);
    js["table_num"] = to_json(10);
    js["bin_width"] = to_json(1.0);
    js["probe_num"] = to_json(1);
    js["seed"] = to_json(0);
    js["retain_projection"] = to_json(false);
    ret.push_back(make_pair("euclid_lsh", common::jsonconfig::config(js)));
  }

  {  // nearest_neighbor_recommender
    json js(new json_object);
    js["method"] = to_json(std::string("minhash"));
    js["parameter"] = json(new json_object);
    js["parameter"]["hash_num"] = to_json(64);
    ret.push_back(make_pair("nearest_neighbor_recommender",
                            common::jsonconfig::config(js)));
    {  // unlearn
      json js_unlearn(js.clone());
      js_unlearn["unlearner"] = to_json(std::string("lru"));
      js_unlearn["unlearner_parameter"] = new json_object;
      js_unlearn["unlearner_parameter"]["max_size"] = to_json(1);
      ret.push_back(
          recommender_parameter(
              "nearest_neighbor_recommender",
              common::jsonconfig::config(js_unlearn)));
    }
  }
  return ret;
}

TEST_P(recommender_factory_test, create_normal) {
  EXPECT_NO_THROW(
    recommender_factory::create_recommender(
      this->name_,
      this->conf_,
      "id"));
}

INSTANTIATE_TEST_CASE_P(
    create_without_unlearner,
    recommender_factory_test,
    ::testing::ValuesIn(generate_parameters()));

TEST(recommender_factory, no_unlearner_parameter) {
  json js(new json_object);
  js["method"] = to_json(std::string("minhash"));
  js["parameter"] = json(new json_object);
  js["parameter"]["hash_num"] = to_json(64);
  js["unlearner"] = to_json(std::string("lru"));
  common::jsonconfig::config conf(js);
  EXPECT_THROW(
    recommender_factory::create_recommender(
      "nearest_neighbor_recommender",
      conf,
      "id"),
    common::config_exception);
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
