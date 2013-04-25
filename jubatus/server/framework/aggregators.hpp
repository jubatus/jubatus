// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_AGGREGATORS_HPP_
#define JUBATUS_SERVER_FRAMEWORK_AGGREGATORS_HPP_

#include <map>
#include <vector>

namespace jubatus {
namespace server {
namespace framework {

template<typename K, typename V>
std::map<K, V> merge(std::map<K, V> lhs, std::map<K, V> rhs) {
  std::map<K, V> ret;
  typename std::map<K, V>::const_iterator it;
  for (it = lhs.begin(); it != lhs.end(); ++it) {
    ret[it->first] = it->second;
  }
  for (it = rhs.begin(); it != rhs.end(); ++it) {
    ret[it->first] = it->second;
  }
  return ret;
}

template<typename T>
std::vector<T> concat(std::vector<T> lhs, std::vector<T> rhs) {
  std::vector<T> ret = lhs;
  ret.insert(ret.end(), rhs.begin(), rhs.end());
  return ret;
}

template<typename T>
T random(T lhs, T rhs) {
  return lhs;  // TODO( ): make random? or left(change fun name)?
}

template<typename T>
T pass(T lhs, T rhs) {
  return lhs;  // TODO( ):
}

template<typename T>
T add(T lhs, T rhs) {
  return lhs + rhs;
}

bool all_and(bool l, bool r) {
  return l && r;
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_AGGREGATORS_HPP_
