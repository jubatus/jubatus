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

#include "classifier_factory.hpp"

#include <string>

#include "classifier.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "../storage/storage_base.hpp"
#include "../unlearner/unlearner_factory.hpp"

using jubatus::core::common::jsonconfig::config;
using jubatus::core::common::jsonconfig::config_cast_check;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace classifier {
namespace {

struct unlearner_config {
  jubatus::util::data::optional<std::string> unlearner;
  jubatus::util::data::optional<config> unlearner_parameter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(unlearner) & JUBA_MEMBER(unlearner_parameter);
  }
};

struct unlearning_classifier_config
    : public classifier_config, unlearner_config {
  template<typename Ar>
  void serialize(Ar& ar) {
    classifier_config::serialize(ar);
    unlearner_config::serialize(ar);
  }
};

jubatus::util::lang::shared_ptr<unlearner::unlearner_base>
create_unlearner(const unlearner_config& conf) {
  if (conf.unlearner) {
    if (!conf.unlearner_parameter) {
      throw JUBATUS_EXCEPTION(common::exception::runtime_error(
          "Unlearner is set but unlearner_parameter is not found"));
    }
    return unlearner::create_unlearner(
        *conf.unlearner, *conf.unlearner_parameter);
  } else {
    return jubatus::util::lang::shared_ptr<unlearner::unlearner_base>();
  }
}

}  // namespace

shared_ptr<classifier_base> classifier_factory::create_classifier(
    const std::string& name,
    const common::jsonconfig::config& param,
    jubatus::util::lang::shared_ptr<storage::storage_base> storage) {
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner;
  shared_ptr<classifier_base> res;
  if (name == "perceptron") {
    // perceptron doesn't have parameter
    if (param.type() != jubatus::util::text::json::json::Null) {
      unlearner_config conf = config_cast_check<unlearner_config>(param);
      unlearner = create_unlearner(conf);
    }
    res.reset(new perceptron(storage));
  } else if (name == "PA" || name == "passive_aggressive") {
    // passive_aggressive doesn't have parameter
    if (param.type() != jubatus::util::text::json::json::Null) {
      unlearner_config conf = config_cast_check<unlearner_config>(param);
      unlearner = create_unlearner(conf);
    }
    res.reset(new passive_aggressive(storage));
  } else if (name == "PA1" || name == "passive_aggressive_1") {
    unlearning_classifier_config conf
        = config_cast_check<unlearning_classifier_config>(param);
    unlearner = create_unlearner(conf);
    res.reset(new passive_aggressive_1(conf, storage));
  } else if (name == "PA2" || name == "passive_aggressive_2") {
    unlearning_classifier_config conf
        = config_cast_check<unlearning_classifier_config>(param);
    unlearner = create_unlearner(conf);
    res.reset(new passive_aggressive_2(conf, storage));
  } else if (name == "CW" || name == "confidence_weighted") {
    unlearning_classifier_config conf
        = config_cast_check<unlearning_classifier_config>(param);
    unlearner = create_unlearner(conf);
    res.reset(new confidence_weighted(conf, storage));
  } else if (name == "AROW" || name == "arow") {
    unlearning_classifier_config conf
        = config_cast_check<unlearning_classifier_config>(param);
    unlearner = create_unlearner(conf);
    res.reset(new arow(conf, storage));
  } else if (name == "NHERD" || name == "normal_herd") {
    unlearning_classifier_config conf
        = config_cast_check<unlearning_classifier_config>(param);
    unlearner = create_unlearner(conf);
    res.reset(new normal_herd(conf, storage));
  } else {
    throw JUBATUS_EXCEPTION(
        common::unsupported_method("classifier(" + name + ")"));
  }

  if (unlearner) {
    res->set_label_unlearner(unlearner);
  }
  return res;
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
