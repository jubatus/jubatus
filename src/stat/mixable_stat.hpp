// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include "stat.hpp"
#include <map>

namespace jubatus {
namespace stat {

class mixable_stat : public stat {
public:
  // entropy = - \sum_{for all keys in this stat} p(key) log p(key)

  // - mixed entropy -
  //p(key) log p(key) = { n(key) log n(key) - n (key) log N } / N
  //E (key) = ( \sum n(key) log n(key) )/N - log N / N \sum n(key)
  //        = ( \sum n(key) log n(key) )/N - log N
  //get_diff : () ->  pair( \sum n(key) log n(key), partial N )
  //put_diff : pair( \sum n(key) log n(key), partial N ) -> ()
  // mix : pair(e1, n1) -> pair(e2, n2) -> pair( e1+e2, n1+n2 )
  // entropy : e -> n -> e/n - log n
  std::pair<double,size_t> get_diff() const ;
  void put_diff(const std::pair<double,size_t>&);
  static void reduce(const std::pair<double,size_t>&,
		     const std::pair<double,size_t>&,
		     std::pair<double,size_t>&);
  double mixed_entropy() const;

private:
  double e_;
  double n_;
};

}
}
