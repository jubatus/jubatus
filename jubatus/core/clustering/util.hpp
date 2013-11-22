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

#ifndef JUBATUS_CORE_CLUSTERING_UTIL_HPP_
#define JUBATUS_CORE_CLUSTERING_UTIL_HPP_

#include <utility>
#include <vector>
#include "types.hpp"
#include "discrete_distribution.hpp"

namespace jubatus {
namespace core {
namespace clustering {

void concat(const wplist& src, wplist& dst);
char digit(int num, int r, int n);

double sum(const common::sfv_t& p);
double sum2(const common::sfv_t& p);

void scalar_mul_and_add(
    const common::sfv_t& left,
    float s,
    common::sfv_t& right);
common::sfv_t add(const common::sfv_t& p1, const common::sfv_t& p2);
common::sfv_t sub(const common::sfv_t& p1, const common::sfv_t& p2);
common::sfv_t scalar_dot(const common::sfv_t& p, double s);

double dist(const common::sfv_t& p1, const common::sfv_t& p2);
double dist(const weighted_point& p1, const weighted_point& p2);
std::pair<size_t, double> min_dist(
    const common::sfv_t& p, const std::vector<common::sfv_t>& P);
std::pair<size_t, double> min_dist(
    const weighted_point& p, const wplist& P);

void dump_wplist(const wplist& src);

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_UTIL_HPP_
