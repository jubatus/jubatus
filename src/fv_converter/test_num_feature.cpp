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

#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "num_feature.hpp"

namespace jubatus {
namespace fv_converter {

class my_num_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      std::vector<std::pair<std::string, float> >& ret_fv) const {
    //  waring: this implementation is INVALID  use value[n].first as index
    if (value.size() == 1) {
      ret_fv.push_back(std::make_pair(key, value[0].second + 1));
    } else {
      for (size_t i = 0; i < value.size(); ++i) {
        std::stringstream ss;
        ss << key << "_" << i;
        ret_fv.push_back(std::make_pair(ss.str(), value[i].second + 1));
      }
    }
  }
};

extern "C" {
  num_feature* create(const std::map<std::string, std::string>& params) {
  return new my_num_feature();
}
}

}  // namespace fv_converter
}  // namespace jubatus
