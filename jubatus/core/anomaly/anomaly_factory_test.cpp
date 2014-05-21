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

#include <gtest/gtest.h>

#include "anomaly_factory.hpp"
#include "../common/jsonconfig.hpp"

using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::to_json;

namespace jubatus {
namespace core {
namespace anomaly {

TEST(anomaly_factory, create_lof) {
  json js(new json_object);
  js["method"] = to_json(std::string("lsh"));
  js["parameter"] = json(new json_object);
  js["parameter"]["hash_num"] = to_json(64);
  js["nearest_neighbor_num"] = to_json(10);
  js["reverse_nearest_neighbor_num"] = to_json(10);
  common::jsonconfig::config conf(js);
  EXPECT_NO_THROW(anomaly_factory::create_anomaly("lof", conf, "id"));
}

TEST(anomaly_factory, create_light_lof) {
  json js(new json_object);
  js["method"] = to_json(std::string("lsh"));
  js["parameter"] = json(new json_object);
  js["parameter"]["hash_num"] = to_json(64);
  js["nearest_neighbor_num"] = to_json(10);
  js["reverse_nearest_neighbor_num"] = to_json(10);
  common::jsonconfig::config conf(js);
  EXPECT_NO_THROW(anomaly_factory::create_anomaly("light_lof", conf, "id"));
}

TEST(anomaly_factory, create_light_lof_with_unlearner) {
  json js(new json_object);
  js["method"] = to_json(std::string("lsh"));
  js["parameter"] = json(new json_object);
  js["parameter"]["hash_num"] = to_json(64);
  js["nearest_neighbor_num"] = to_json(10);
  js["reverse_nearest_neighbor_num"] = to_json(10);
  js["unlearner"] = to_json(std::string("lru"));
  js["unlearner_parameter"] = new json_object;
  js["unlearner_parameter"]["max_size"] = to_json(1);
  common::jsonconfig::config conf(js);
  EXPECT_NO_THROW(anomaly_factory::create_anomaly("light_lof", conf, "id"));
}

TEST(anomaly_factory, create_lof_with_unlearner) {
  // LOF does not support unleaner
  json js(new json_object);
  js["method"] = to_json(std::string("lsh"));
  js["parameter"] = json(new json_object);
  js["parameter"]["hash_num"] = to_json(64);
  js["nearest_neighbor_num"] = to_json(10);
  js["reverse_nearest_neighbor_num"] = to_json(10);
  js["unlearner"] = to_json(std::string("lru"));
  js["unlearner_parameter"] = new json_object;
  js["unlearner_parameter"]["max_size"] = to_json(1);
  common::jsonconfig::config conf(js);
  EXPECT_THROW(anomaly_factory::create_anomaly("lof", conf, "id"),
               common::jsonconfig::cast_check_error);
}



}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
