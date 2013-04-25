// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "revert.hpp"

#include <string>
#include <utility>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>
#include "datum.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

void revert_feature(const sfv_t& fv, fv_converter::datum& data) {
  for (size_t i = 0; i < fv.size(); ++i) {
    std::pair<std::string, float> num_value;
    std::pair<std::string, std::string> string_value;
    if (revert_num_value(fv[i], num_value)) {
      data.num_values_.push_back(num_value);
    } else if (revert_string_value(fv[i], string_value)) {
      data.string_values_.push_back(string_value);
    }
  }
}

bool revert_num_value(
    const std::pair<std::string, float>& feature,
    std::pair<std::string, float>& num_value) {
  // Only 'num' features and 'str' features can be reverted.
  // Formats of two features are below:
  // ("<KEY_NAME>@num", value)
  // ("<KEY_NAME>@str$<VALUE>", 1)
  const std::string& key = feature.first;
  float value = feature.second;
  size_t at = key.rfind('@');
  if (at == std::string::npos) {
    return false;
  }
  std::string num_value_key(key.substr(0, at));
  std::string feature_value(key.substr(at + 1));
  const std::string str_prefix = "str$";
  if (feature_value == "num") {
    num_value.first.swap(num_value_key);
    num_value.second = value;
    return true;
  } else if (pfi::data::string::starts_with(feature_value, str_prefix)) {
    std::string val_string(feature_value.substr(str_prefix.size()));
    try {
      float val = pfi::lang::lexical_cast<float>(val_string);
      num_value.first.swap(num_value_key);
      num_value.second = val;
      return true;
    } catch (const std::bad_cast&) {
      return false;
    }
  } else {
    return false;
  }
}

bool revert_string_value(
    const std::pair<std::string, float>& feature,
    std::pair<std::string, std::string>& string_value) {
  // Format of string feature is
  // "<KEY_NAME>$<VALUE>@<FEATURE_TYPE>#<SAMPLE_WEIGHT>/<GLOBAL_WEIGHT>"
  const std::string& f = feature.first;
  size_t sharp = f.rfind('#');
  if (sharp == std::string::npos) {
    return false;
  }
  size_t at = f.rfind('@', sharp);
  if (at == std::string::npos) {
    return false;
  }
  size_t dollar = f.find('$');
  if (dollar == std::string::npos) {
    return false;
  }
  if (f.substr(at + 1, sharp - at - 1) != "str") {
    return false;
  }

  std::string key(f.substr(0, dollar));
  std::string value(f.substr(dollar + 1, at - dollar - 1));

  string_value.first.swap(key);
  string_value.second.swap(value);
  return true;
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
