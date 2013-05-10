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

#ifndef JUBATUS_CORE_FV_CONVERTER_CONVERTER_CONFIG_HPP_
#define JUBATUS_CORE_FV_CONVERTER_CONVERTER_CONFIG_HPP_

#include <map>
#include <string>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/optional.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/text/json.h>

namespace jubatus {
namespace core {
namespace fv_converter {

class datum_to_fv_converter;

typedef std::map<std::string, std::string> param_t;

struct string_rule {
  std::string key;
  pfi::data::optional<std::string> except;
  std::string type;
  std::string sample_weight;
  std::string global_weight;

  friend class pfi::data::serialization::access;
  template<class Archive>
  void serialize(Archive& ar) {
    ar
        & MEMBER(key)
        & MEMBER(except)
        & MEMBER(type)
        & MEMBER(sample_weight)
        & MEMBER(global_weight);
  }
};

struct filter_rule {
  std::string key;
  pfi::data::optional<std::string> except;
  std::string type;
  std::string suffix;

  friend class pfi::data::serialization::access;
  template<class Archive>
  void serialize(Archive& ar) {
    ar & MEMBER(key) & MEMBER(except) & MEMBER(type) & MEMBER(suffix);
  }
};

struct num_rule {
  std::string key;
  pfi::data::optional<std::string> except;
  std::string type;

  friend class pfi::data::serialization::access;
  template<class Archive>
  void serialize(Archive& ar) {
    ar & MEMBER(key) & MEMBER(except) & MEMBER(type);
  }
};

struct converter_config {
  std::map<std::string, param_t> string_filter_types;
  std::vector<filter_rule> string_filter_rules;

  std::map<std::string, param_t> num_filter_types;
  std::vector<filter_rule> num_filter_rules;

  std::map<std::string, param_t> string_types;
  std::vector<string_rule> string_rules;

  std::map<std::string, param_t> num_types;
  std::vector<num_rule> num_rules;

  pfi::data::optional<int64_t> hash_max_size;

  friend class pfi::data::serialization::access;
  template<class Archive>
  void serialize(Archive& ar) {
    ar & MEMBER(string_filter_types) & MEMBER(string_filter_rules)
        & MEMBER(num_filter_types) & MEMBER(num_filter_rules)
        & MEMBER(string_types) & MEMBER(string_rules) & MEMBER(num_types)
        & MEMBER(num_rules) & MEMBER(hash_max_size);
  }
};

void initialize_converter(
    const converter_config& config,
    datum_to_fv_converter& converter);

pfi::lang::shared_ptr<datum_to_fv_converter>
make_fv_converter(const std::string& config);

pfi::lang::shared_ptr<datum_to_fv_converter>
make_fv_converter(const pfi::text::json::json& config);

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_CONVERTER_CONFIG_HPP_
