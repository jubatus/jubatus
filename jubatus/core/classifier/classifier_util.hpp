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

#ifndef JUBATUS_CORE_CLASSIFIER_CLASSIFIER_UTIL_HPP_
#define JUBATUS_CORE_CLASSIFIER_CLASSIFIER_UTIL_HPP_

#include <string>

namespace jubatus {
namespace core {
namespace classifier {

class ClassifierUtil {
 public:
  template<class T, class U>
  static void get_two(
      const T& t,
      const std::string& label1,
      const std::string& label2,
      U& u1,
      U& u2) {
    for (size_t i = 0; i < t.size(); ++i) {
      if (t[i].first == label1) {
        u1 = t[i].second;
      } else if (t[i].first == label2) {
        u2 = t[i].second;
      }
    }
  }
};

}  // namespace jubatus
}  // namespace core
}  // namespace classifier

#endif  // JUBATUS_CORE_CLASSIFIER_CLASSIFIER_UTIL_HPP_
