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

#include "weight_manager.hpp"

#include <cmath>
#include <string>
#include <utility>
#include "../common/type.hpp"
#include "datum_to_fv_converter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

struct is_zero {
  bool operator()(const std::pair<std::string, float>& p) {
    return p.second == 0;
  }
};

}  // namespace

weight_manager::weight_manager()
    : diff_weights_(),
      master_weights_() {
}

void weight_manager::update_weight(const sfv_t& fv) {
  diff_weights_.update_document_frequency(fv);
}

void weight_manager::get_weight(sfv_t& fv) const {
  for (sfv_t::iterator it = fv.begin(); it != fv.end(); ++it) {
    double global_weight = get_global_weight(it->first);
    it->second *= global_weight;
  }
  fv.erase(remove_if(fv.begin(), fv.end(), is_zero()), fv.end());
}

double weight_manager::get_global_weight(const std::string& key) const {
  size_t p = key.find_last_of('/');
  if (p == std::string::npos) {
    return 1.0;
  }
  std::string type = key.substr(p + 1);
  if (type == "bin") {
    return 1.0;
  } else if (type == "idf") {
    double doc_count = get_document_count();
    double doc_freq = get_document_frequency(key);
    return log((doc_count + 1) / (doc_freq + 1));
  } else if (type == "weight") {
    p = key.find_last_of('#');
    if (p == std::string::npos) {
      return 0;
    } else {
      return get_user_weight(key.substr(0, p));
    }
  } else {
    return 1;
  }
}

void weight_manager::add_weight(const std::string& key, float weight) {
  diff_weights_.add_weight(key, weight);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
