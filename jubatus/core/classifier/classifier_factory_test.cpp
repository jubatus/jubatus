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

#include "classifier_factory.hpp"
#include "classifier_base.hpp"
#include "../common/jsonconfig.hpp"
#include "../storage/local_storage.hpp"

using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::to_json;

namespace jubatus {
namespace core {
namespace classifier {


TEST(classifier_factory, create_without_unlearner) {
  classifier_base::storage_ptr s(new storage::local_storage);

  {
    json js(new json_object);
    common::jsonconfig::config conf(js);

    EXPECT_NO_THROW(classifier_factory::create_classifier("perceptron", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("PA", conf, s));
  }
  {
    json js(new json_object);
    js["regularization_weight"] = to_json(1.0);
    common::jsonconfig::config conf(js);

    EXPECT_NO_THROW(classifier_factory::create_classifier("PA1", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("PA2", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("CW", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("AROW", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("NHERD", conf, s));
  }
}


TEST(classifier_factory, create_with_unlearner) {
  classifier_base::storage_ptr s(new storage::local_storage);

  {
    json js(new json_object);
    js["unlearner"] = to_json(std::string("lru"));
    js["unlearner_parameter"] = new json_object;
    js["unlearner_parameter"]["max_size"] = to_json(1);
    common::jsonconfig::config conf(js);

    EXPECT_NO_THROW(classifier_factory::create_classifier("perceptron", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("PA", conf, s));
  }
  {
    json js(new json_object);
    js["regularization_weight"] = to_json(1.0);
    js["unlearner"] = to_json(std::string("lru"));
    js["unlearner_parameter"] = new json_object;
    js["unlearner_parameter"]["max_size"] = to_json(1);
    common::jsonconfig::config conf(js);

    EXPECT_NO_THROW(classifier_factory::create_classifier("PA1", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("PA2", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("CW", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("AROW", conf, s));
    EXPECT_NO_THROW(classifier_factory::create_classifier("NHERD", conf, s));
  }
}

// --- validation test ---

TEST(classifier_factory, invalid_unlearner_config) {
  classifier_base::storage_ptr s(new storage::local_storage);
  json js(new json_object);
  js["unlearner"] = to_json(std::string("lru"));
  common::jsonconfig::config conf(js);

  EXPECT_THROW(classifier_factory::create_classifier("perceptron", conf, s),
               common::exception::runtime_error);
  EXPECT_THROW(classifier_factory::create_classifier("PA", conf, s),
               common::exception::runtime_error);
}

TEST(classifier_factory, invalid_unlearner_name) {
  classifier_base::storage_ptr s(new storage::local_storage);
  json js(new json_object);
  js["regularization_weight"] = to_json(1.0);
  js["unlearner"] = to_json(std::string("Jubatus"));  // not exist
  js["unlearner_parameter"] = new json_object;
  js["unlearner_parameter"]["max_size"] = to_json(1);
  common::jsonconfig::config param(js);

  EXPECT_THROW(classifier_factory::create_classifier("PA1", param, s),
               common::unsupported_method);
  EXPECT_THROW(classifier_factory::create_classifier("PA2", param, s),
               common::unsupported_method);
  EXPECT_THROW(classifier_factory::create_classifier("AROw", param, s),
               common::unsupported_method);
}

TEST(classifier_factory, invalid_unlearner_parameter) {
  classifier_base::storage_ptr s(new storage::local_storage);
  json js(new json_object);
  js["regularization_weight"] = to_json(1.0);
  js["unlearner"] = to_json(std::string("lru"));
  common::jsonconfig::config conf(js);

  EXPECT_THROW(classifier_factory::create_classifier("PA1", conf, s),
               common::exception::runtime_error);
  EXPECT_THROW(classifier_factory::create_classifier("PA2", conf, s),
               common::exception::runtime_error);
  EXPECT_THROW(classifier_factory::create_classifier("CW", conf, s),
               common::exception::runtime_error);
  EXPECT_THROW(classifier_factory::create_classifier("AROW", conf, s),
               common::exception::runtime_error);
  EXPECT_THROW(classifier_factory::create_classifier("NHERD", conf, s),
               common::exception::runtime_error);
}

TEST(classifier_factory, invalid_algorithm) {
  classifier_base::storage_ptr s(new storage::local_storage);

  {  // invalid classifier name
    json js(new json_object);
    js["regularization_weight"] = to_json(1.0);
    js["unlearner"] = to_json(std::string("lru"));
    js["unlearner_parameter"] = new json_object;
    js["unlearner_parameter"]["max_size"] = to_json(1);
    common::jsonconfig::config param(js);

    EXPECT_THROW(classifier_factory::create_classifier("pa", param, s),
                 common::unsupported_method);
    EXPECT_THROW(classifier_factory::create_classifier("", param, s),
                 common::unsupported_method);
    EXPECT_THROW(classifier_factory::create_classifier("saitama", param, s),
                 common::unsupported_method);
  }
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
