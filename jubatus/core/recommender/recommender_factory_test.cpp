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

#include "recommender_factory.hpp"
#include "../common/jsonconfig.hpp"

using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::to_json;

namespace jubatus {
namespace core {
namespace recommender {

TEST(recommender_factory, create_without_unlearner) {
  {
    json js(new json_object);
    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "inverted_index",
            conf,
            "id"));
  }
  {
    json js(new json_object);
    js["hash_num"] = to_json(64);
    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "minhash",
            conf,
            "id"));
  }
  {
    json js(new json_object);
    js["hash_num"] = to_json(64);
    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "lsh",
            conf,
            "id"));
  }
  {
    json js(new json_object);
    js["hash_num"] = to_json(64);
    js["table_num"] = to_json(10);
    js["bin_width"] = to_json(1.0);
    js["probe_num"] = to_json(1);
    js["seed"] = to_json(0);
    js["retain_projection"] = to_json(false);

    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "euclid_lsh",
            conf,
            "id"));
  }
  {
    json js(new json_object);
    js["method"] = to_json(std::string("minhash"));
    js["parameter"] = json(new json_object);
    js["parameter"]["hash_num"] = to_json(64);
    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "nearest_neighbor_recommender",
            conf,
            "id"));
  }
}

TEST(recommender_factory, create_with_unlearner) {
  {
    json js(new json_object);
    js["method"] = to_json(std::string("minhash"));
    js["parameter"] = json(new json_object);
    js["parameter"]["hash_num"] = to_json(64);
    js["unlearner"] = to_json(std::string("lru"));
    js["unlearner_parameter"] = new json_object;
    js["unlearner_parameter"]["max_size"] = to_json(1);
    common::jsonconfig::config conf(js);
    EXPECT_NO_THROW(
        recommender_factory::create_recommender(
            "nearest_neighbor_recommender",
            conf,
            "id"));
  }
}

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
