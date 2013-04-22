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

#ifndef JUBATUS_CORE_RECOMMENDER_LSH_UTIL_HPP_
#define JUBATUS_CORE_RECOMMENDER_LSH_UTIL_HPP_

#include <vector>
#include <string>
#include <pficommon/data/unordered_map.h>
#include "../common/type.hpp"

namespace jubatus {
namespace core {

namespace storage {
class bit_vector;
}  // namespace storage

namespace recommender {

void generate_random_vector(size_t dim, uint32_t seed, std::vector<float>& v);

void set_bit_vector(
    const std::vector<float>& vec,
    jubatus::core::storage::bit_vector& bit_vec);

void prod_invert_and_vector(
    const pfi::data::unordered_map<std::string, std::vector<float> >& matrix,
    const sfv_t& vec,
    size_t dim,
    std::vector<float>& ret);

}  // namespace recommender

}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_LSH_UTIL_HPP_
