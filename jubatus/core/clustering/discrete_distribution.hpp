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

#ifndef JUBATUS_CORE_CLUSTERING_DISCRETE_DISTRIBUTION_HPP_
#define JUBATUS_CORE_CLUSTERING_DISCRETE_DISTRIBUTION_HPP_

#include <vector>
#include <pficommon/math/random.h>

namespace jubatus {
namespace core {
namespace clustering {

class discrete_distribution {
 public:
  discrete_distribution(
      std::vector<double>::iterator begin,
      std::vector<double>::iterator end);

  size_t operator() ();

 private:
  size_t convert_(double d);
  pfi::math::random::mtrand r_;
  std::vector<double> whist_;
  double sum_;
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_DISCRETE_DISTRIBUTION_HPP_
