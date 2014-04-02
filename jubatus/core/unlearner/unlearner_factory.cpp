// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "unlearner_factory.hpp"

#include <string>
#include "../common/exception.hpp"
#include "lru_unlearner.hpp"
#include "random_unlearner.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

unlearner_base* create_unlearner(
    const std::string& name,
    const common::jsonconfig::config& config) {
  if (name == "lru") {
    return new lru_unlearner(
        common::jsonconfig::config_cast_check<lru_unlearner::config>(config));
  } else if (name == "random") {
    return new random_unlearner(
        common::jsonconfig::config_cast_check<random_unlearner::config>(
            config));
  }
  throw JUBATUS_EXCEPTION(common::unsupported_method(
      "unlearner(" + name + ')'));
}

}  // namespace unlearner
}  // namespace core
}  // namepsace jubatus
