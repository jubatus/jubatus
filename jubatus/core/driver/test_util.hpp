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

#ifndef JUBATUS_CORE_DRIVER_TEST_UTIL_HPP_
#define JUBATUS_CORE_DRIVER_TEST_UTIL_HPP_

#include <string>
#include <sstream>
#include <vector>

#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../framework/mixable.hpp"


pfi::lang::shared_ptr<jubatus::core::fv_converter::datum_to_fv_converter>
  make_fv_converter() {
  pfi::lang::shared_ptr<jubatus::core::fv_converter::datum_to_fv_converter>
    converter(
        new jubatus::core::fv_converter::datum_to_fv_converter);

  jubatus::core::fv_converter::string_rule str_rule;
  str_rule.key = "*";
  str_rule.type = "str";
  str_rule.sample_weight = "bin";
  str_rule.global_weight = "bin";
  jubatus::core::fv_converter::num_rule num_rule;
  num_rule.key = "*";
  num_rule.type = "num";

  jubatus::core::fv_converter::converter_config c;
  c.string_rules.push_back(str_rule);
  c.num_rules.push_back(num_rule);

  jubatus::core::fv_converter::initialize_converter(c, *converter);
  return converter;
}

void save_model(pfi::lang::shared_ptr<
  jubatus::core::framework::mixable_holder>
  holder, std::string& data) {
  std::stringstream os;
  std::vector<jubatus::core::framework::mixable0*> mixables =
    holder->get_mixables();
  for (size_t i = 0; i < mixables.size(); ++i) {
    mixables[i]->save(os);
  }
  data = os.str();
}

void load_model(pfi::lang::shared_ptr<
  jubatus::core::framework::mixable_holder> holder,
  const std::string& data) {
  std::stringstream is(data);
  std::vector<jubatus::core::framework::mixable0*> mixables =
    holder->get_mixables();
  for (size_t i = 0; i < mixables.size(); ++i) {
    mixables[i]->clear();
    mixables[i]->load(is);
  }
}

#endif  // JUBATUS_CORE_DRIVER_TEST_UTIL_HPP_
