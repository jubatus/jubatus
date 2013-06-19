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

using jubatus::core::common::jsonconfig::config_cast_check;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace classifier {

shared_ptr<classifier_base> classifier_factory::create_classifier(
    const std::string& name,
    const common::jsonconfig::config& param,
    pfi::lang::shared_ptr<storage::storage_base> storage) {
  if (name == "perceptron") {
    // perceptron doesn't have parameter
    return shared_ptr<classifier_base>(new perceptron(storage));
  } else if (name == "PA" || name == "passive_aggressive") {
    // passive_aggressive doesn't have parameter
    return shared_ptr<classifier_base>(new passive_aggressive(storage));
  } else if (name == "PA1" || name == "passive_aggressive_1") {
    return shared_ptr<classifier_base>(new passive_aggressive_1(
        config_cast_check<classifier_config>(param), storage));
  } else if (name == "PA2" || name == "passive_aggressive_2") {
    return shared_ptr<classifier_base>(new passive_aggressive_2(
        config_cast_check<classifier_config>(param), storage));
  } else if (name == "CW" || name == "confidence_weighted") {
    return shared_ptr<classifier_base>(new confidence_weighted(
        config_cast_check<classifier_config>(param), storage));
  } else if (name == "AROW" || name == "arow") {
    return shared_ptr<classifier_base>(new arow(
        config_cast_check<classifier_config>(param), storage));
  } else if (name == "NHERD" || name == "normal_herd") {
    return shared_ptr<classifier_base>(new normal_herd(
        config_cast_check<classifier_config>(param), storage));
  } else {
    throw JUBATUS_EXCEPTION(common::unsupported_method(name));
  }
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
