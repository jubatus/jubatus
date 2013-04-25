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

#include "storage_base.hpp"
#include <string>
#include <pficommon/text/json.h>

using std::string;

namespace jubatus {
namespace core {
namespace storage {

void storage_base::update(
    const string& feature,
    const string& inc_class,
    const string& dec_class, const val1_t& w) {
  feature_val1_t row;
  get(feature, row);
  float inc_class_val = w;
  float dec_class_val = -w;
  for (size_t i = 0; i < row.size(); ++i) {
    const string& label = row[i].first;
    if (label == inc_class) {
      inc_class_val += row[i].second;
    } else if (label == dec_class) {
      dec_class_val += row[i].second;
    }
  }
  set(feature, inc_class, inc_class_val);
  set(feature, dec_class, dec_class_val);
}

void storage_base::inp(const sfv_t& sfv, map_feature_val1_t& ret) {
  ret.clear();
  for (sfv_t::const_iterator it = sfv.begin(); it != sfv.end(); ++it) {
    const string& feature = it->first;
    const float val = it->second;
    feature_val1_t fval1;
    get(feature, fval1);
    for (feature_val1_t::const_iterator it2 = fval1.begin(); it2 != fval1.end();
        ++it2) {
      ret[it2->first] += it2->second * val;
    }
  }
}

void storage_base::bulk_update(
    const sfv_t& sfv,
    float step_width,
    const std::string& inc_class,
    const std::string& dec_class) {
  for (sfv_t::const_iterator it = sfv.begin(); it != sfv.end(); ++it) {
    const string& feature = it->first;
    float val = it->second;
    if (dec_class != "") {
      update(feature, inc_class, dec_class, step_width * val);
    } else {
      feature_val1_t ret;
      get(feature, ret);
      float pos_val = 0.f;
      for (size_t i = 0; i < ret.size(); ++i) {
        if (ret[i].first == inc_class) {
          pos_val = ret[i].second;
          break;
        }
      }
      set(feature, inc_class, pos_val + step_width * val);
    }
  }
}

void storage_base::get_diff(features3_t& v) const {
  v.clear();
}

void storage_base::set_average_and_clear_diff(const features3_t&) {
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
