// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA


#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>

#include "datum.hpp"
#include "revert.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;

void revert_feature(const sfv_t& fv,
                    fv_converter::datum& data) {
  for (size_t i = 0; i < fv.size(); ++i) {
    pair<string, float> num_value;
    pair<string, string> string_value;
    if (revert_num_value(fv[i], num_value)) {
      data.num_values_.push_back(num_value);
    } else if (revert_string_value(fv[i], string_value)) {
      data.string_values_.push_back(string_value);
    }
  }
}


bool revert_num_value(const pair<string, float>& feature,
                      pair<string, float>& num_value) {
  // Only 'num' features and 'str' features can be reverted.
  // Formats of two features are below:
  // ("<KEY_NAME>@num", value)
  // ("<KEY_NAME>@str$<VALUE>", 1)
  const string& key = feature.first;
  float value = feature.second;
  size_t at = key.rfind('@');
  if (at == string::npos) {
    return false;
  }
  string num_value_key(key.substr(0, at));
  string feature_value(key.substr(at + 1));
  const string str_prefix = "str$";
  if (feature_value == "num") {
    num_value.first.swap(num_value_key);
    num_value.second = value;
    return true;
  } else if (pfi::data::string::starts_with(feature_value, str_prefix)) {
    string val_string(feature_value.substr(str_prefix.size()));
    try {
      float val = pfi::lang::lexical_cast<float>(val_string);
      num_value.first.swap(num_value_key);
      num_value.second = val;
      return true;
    } catch (const bad_cast&) {
      return false;
    }
  } else {
    return false;
  }
}

bool revert_string_value(const pair<string, float>& feature,
                         pair<string, string>& string_value) {
  // Format of string feature is
  // "<KEY_NAME>$<VALUE>@<FEATURE_TYPE>#<SAMPLE_WEIGHT>/<GLOBAL_WEIGHT>"
  const string& f = feature.first;
  size_t sharp = f.rfind('#');
  if (sharp == string::npos) {
    return false;
  }
  size_t at = f.rfind('@', sharp);
  if (at == string::npos) {
    return false;
  }
  size_t dollar = f.rfind('$', at);
  if (dollar == string::npos) {
    return false;
  }
  if (f.substr(at + 1, sharp - at - 1) != "str") {
    return false;
  }
  
  string key(f.substr(0, dollar));
  string value(f.substr(dollar + 1, at - dollar - 1));
  
  string_value.first.swap(key);
  string_value.second.swap(value);
  return true;
}

}
}

