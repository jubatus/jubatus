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

#ifndef JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_RANKING_HPP_
#define JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_RANKING_HPP_

#include <vector>
#include <utility>
#include "../table/column/bit_vector.hpp"
#include "../table/column/columns.hpp"

namespace jubatus {
namespace nearest_neighbor {

void ranking_hamming_bit_vectors(const table::bit_vector& query,
                                 const table::const_bit_vector_column& bvs,
                                 std::vector<std::pair<uint64_t, float> >& ret,
                                 uint64_t ret_num);

}  // namespace nearest_neighbor
}  // namespace jubatus

#endif  // JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_RANKING_HPP_
