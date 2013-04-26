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

#ifndef JUBATUS_CORE_FV_CONVERTER_COUNTER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_COUNTER_HPP_

#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "../common/unordered_map.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

template<class T>
class counter {
 public:
  typedef pfi::data::unordered_map<T, unsigned> map_t;
  typedef typename pfi::data::unordered_map<T, unsigned>::const_iterator
      const_iterator;
  typedef typename pfi::data::unordered_map<T, unsigned>::iterator iterator;

  bool contains(const T& key) const {
    return data_.count(key) != 0;
  }

  unsigned operator[](const T& key) const {
    const_iterator it = data_.find(key);
    if (it == data_.end()) {
      return 0;
    } else {
      return it->second;
    }
  }

  unsigned& operator[](const T& key) {
    if (data_.count(key) == 0) {
      data_[key] = 0;
    }
    return data_[key];
  }

  const_iterator begin() const {
    return data_.begin();
  }

  iterator begin() {
    return data_.begin();
  }

  const_iterator end() const {
    return data_.end();
  }

  iterator end() {
    return data_.end();
  }

  void clear() {
    pfi::data::unordered_map<T, unsigned>().swap(data_);
  }

  void add(const counter<T>& counts) {
    for (const_iterator it = counts.begin(); it != counts.end(); ++it) {
      (*this)[it->first] += it->second;
    }
  }

  MSGPACK_DEFINE(data_);

  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & MEMBER(data_);
  }

 private:
  pfi::data::unordered_map<T, unsigned> data_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_COUNTER_HPP_
