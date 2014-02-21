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

#ifndef JUBATUS_CORE_CLUSTERING_TYPES_HPP_
#define JUBATUS_CORE_CLUSTERING_TYPES_HPP_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <msgpack.hpp>
#include "jubatus/util/data/serialization.h"

#include "../common/type.hpp"
#include "../common/exception.hpp"
#include "../fv_converter/datum.hpp"

namespace jubatus {
namespace core {
namespace clustering {

typedef double cluster_weight;
typedef std::vector<std::pair<cluster_weight,
            jubatus::core::fv_converter::datum> > cluster_unit;
typedef std::vector<cluster_unit> cluster_set;

class no_cluster_exception
  : public common::exception::jubaexception<no_cluster_exception> {
 public:
  explicit no_cluster_exception(const std::string& msg)
    : what_(msg) {}
  const char *what() const throw() {
    return what_.c_str();
  }
  ~no_cluster_exception() throw() {}

 private:
  std::string what_;
};

struct weighted_point {
 public:
  MSGPACK_DEFINE(weight, data, original, free_double, free_long);
  double weight;
  common::sfv_t data;
  fv_converter::datum original;
  double free_double;
  uint64_t free_long;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(weight)
        & JUBA_MEMBER(data)
        & JUBA_MEMBER(original.num_values_)
        & JUBA_MEMBER(original.string_values_)
        & JUBA_MEMBER(free_double)
        & JUBA_MEMBER(free_long);
  }
};

inline void swap(weighted_point& p1, weighted_point& p2) {
  using std::swap;
  swap(p1.weight, p2.weight);
  swap(p1.data, p2.data);
  swap(p1.original.string_values_, p2.original.string_values_);
  swap(p1.original.num_values_, p2.original.num_values_);
  swap(p1.free_double, p2.free_double);
  swap(p1.free_long, p2.free_long);
}

typedef std::vector<weighted_point> wplist;
typedef std::vector<common::sfv_t> plist;
typedef std::vector<std::pair<std::string, wplist> > diff_t;

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_TYPES_HPP_
