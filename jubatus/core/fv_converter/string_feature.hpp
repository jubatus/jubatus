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

#ifndef JUBATUS_CORE_FV_CONVERTER_STRING_FEATURE_HPP_
#define JUBATUS_CORE_FV_CONVERTER_STRING_FEATURE_HPP_

#include <string>
#include <vector>

namespace jubatus {
namespace core {
namespace fv_converter {

struct string_feature_element {
  size_t begin;
  size_t length;
  std::string value;
  double score;

  string_feature_element(
      size_t begin,
      size_t length,
      const std::string& value,
      double score)
      : begin(begin), length(length), value(value), score(score) {
  }
};

class string_feature {
 public:
  virtual ~string_feature() {
  }

  virtual void extract(
      const std::string& text,
      std::vector<string_feature_element>& result) const = 0;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus


#endif  // JUBATUS_CORE_FV_CONVERTER_STRING_FEATURE_HPP_
