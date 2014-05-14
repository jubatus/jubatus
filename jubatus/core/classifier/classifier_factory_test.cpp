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

#include "classifier_factory.hpp"
#include "../common/jsonconfig.hpp"
#include "../storage/local_storage.hpp"

using jubatus::util::text::json::json;
using jubatus::util::text::json::json_object;
using jubatus::util::text::json::to_json;

namespace jubatus {
namespace core {
namespace classifier {

TEST(classifier_factory, create_without_unlearner) {
  jubatus::util::lang::shared_ptr<storage::storage_base>
      s(new storage::local_storage);

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
  jubatus::util::lang::shared_ptr<storage::storage_base>
      s(new storage::local_storage);

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

TEST(classifier_factory, invalid_unlearner) {
  jubatus::util::lang::shared_ptr<storage::storage_base>
      s(new storage::local_storage);

  {
    json js(new json_object);
    js["unlearner"] = to_json(std::string("lru"));
    common::jsonconfig::config conf(js);
  
    EXPECT_THROW(classifier_factory::create_classifier("perceptron", conf, s),
                 common::exception::runtime_error);
    EXPECT_THROW(classifier_factory::create_classifier("PA", conf, s),
                 common::exception::runtime_error);
  }
  {
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
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
