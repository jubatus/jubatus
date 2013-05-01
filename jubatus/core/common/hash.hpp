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

#ifndef JUBATUS_CORE_COMMON_HASH_HPP_
#define JUBATUS_CORE_COMMON_HASH_HPP_

#include <stdint.h>
#include <string>

namespace jubatus {

class hash_util {
 public:
  static uint64_t calc_string_hash(const std::string& s) {
    // FNV-1 hash function
    uint64_t hash = 14695981039346656037LLU;
    for (size_t i = 0; i < s.size(); ++i) {
      hash *= 1099511628211LLU;
      hash ^= s[i];
    }
    return hash;
  }
};

}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_HASH_HPP_
