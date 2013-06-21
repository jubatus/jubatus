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

#ifndef JUBATUS_CORE_STAT_MIXABLE_STAT_HPP_
#define JUBATUS_CORE_STAT_MIXABLE_STAT_HPP_

#include <map>
#include <string>
#include <utility>

#include <pficommon/data/serialization.h>

#include "stat.hpp"

namespace jubatus {
namespace core {
namespace stat {

class mixable_stat : public stat {
 public:
  explicit mixable_stat(size_t);
  virtual ~mixable_stat();
  // entropy = - \sum_{for all keys in this stat} p(key) log p(key)

  // - mixed entropy -
  // p(key) log p(key) = { n(key) log n(key) - n (key) log N } / N
  // E (key) = ( \sum n(key) log n(key) )/N - log N / N \sum n(key)
  //        = ( \sum n(key) log n(key) )/N - log N
  // get_diff : () ->  pair( \sum n(key) log n(key), partial N )
  // put_diff : pair( \sum n(key) log n(key), partial N ) -> ()
  // mix : pair(e1, n1) -> pair(e2, n2) -> pair( e1+e2, n1+n2 )
  // entropy : e -> n -> e/n - log n
  std::pair<double, size_t> get_diff() const;
  void put_diff(const std::pair<double, size_t>&);
  static void reduce(
      const std::pair<double, size_t>&,
      std::pair<double, size_t>&);
  double entropy() const;

  bool save(std::ostream&);
  bool load(std::istream&);

 private:
  friend class pfi::data::serialization::access;
  template<class Archive>
  void serialize(Archive& ar) {
    ar & e_ & n_;
  }
  double e_;
  double n_;
};
}  // namespace stat
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STAT_MIXABLE_STAT_HPP_
