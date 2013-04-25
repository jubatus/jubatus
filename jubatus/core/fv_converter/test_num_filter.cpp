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
#include <string>
#include "num_filter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

class my_num_filter : public num_filter {
 public:
  double filter(double value) const {
    return value * 2;
  }
};

extern "C" {
num_filter* create(const std::map<std::string, std::string>& params) {
  return new my_num_filter();
}
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
