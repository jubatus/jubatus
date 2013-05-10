// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_STORAGE_LSH_VECTOR_HPP_
#define JUBATUS_CORE_STORAGE_LSH_VECTOR_HPP_

#include <stdint.h>
#include <ostream>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>

namespace jubatus {
namespace core {
namespace storage {

class lsh_vector {
 public:
  lsh_vector();
  lsh_vector(const lsh_vector& lv);
  explicit lsh_vector(size_t len);
  explicit lsh_vector(const std::vector<int>& v);
  ~lsh_vector();

  bool operator==(const lsh_vector& lv) const;
  bool operator!=(const lsh_vector& lv) const;

  int get(size_t pos) const;
  void set(size_t pos, int value);
  void push_back(int value);

  void resize_and_clear(size_t sz);
  size_t size() const;

  lsh_vector slice(size_t from, size_t len) const;
  lsh_vector cut(size_t from, size_t len) const;

  void debug_print(std::ostream& os) const;

  void swap(lsh_vector& lv);

 private:
  friend class pfi::data::hash<lsh_vector>;
  friend class pfi::data::serialization::access;

  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(values_);
  }

  std::vector<int> values_;
};

inline void swap(lsh_vector& l, lsh_vector& r) {  // NOLINT
  l.swap(r);
}

inline std::ostream& operator<<(std::ostream& os, const lsh_vector& lv) {
  lv.debug_print(os);
  return os;
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus

namespace pfi {
namespace data {

template<>
class hash<jubatus::core::storage::lsh_vector> {
 public:
  uint64_t operator()(const jubatus::core::storage::lsh_vector& lv) const {
    const char* p = reinterpret_cast<const char*>(&lv.values_[0]);
    const size_t len = lv.size() * sizeof(lv.values_[0]);
    const char* const end = p + len;

    uint64_t x = 14695981039346656037ull;
    while (p != end) {
      x *= 1099511628211ull;
      x ^= static_cast<uint64_t>(*p++);
    }
    return x;
  }
};

}  // namespace data
}  // namespace pfi

#endif  // JUBATUS_CORE_STORAGE_LSH_VECTOR_HPP_
