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

#include <functional>
#include <utility>
#include <vector>
#include "bit_vector_ranking.hpp"

#include "../storage/fixed_size_heap.hpp"

using std::pair;
using std::vector;
using std::make_pair;
using jubatus::table::bit_vector;
using jubatus::table::const_bit_vector_column;

namespace jubatus {
namespace nearest_neighbor {

void ranking_hamming_bit_vectors(const bit_vector& query,
                                 const const_bit_vector_column& bvs,
                                 vector<pair<uint64_t, float> >& ret,
                                 uint64_t ret_num) {
  storage::fixed_size_heap<pair<uint32_t, uint64_t> > heap(ret_num);
  for (uint64_t i = 0; i< bvs.size(); ++i) {
    const size_t dist = query.calc_hamming_distance(bvs[i]);
    heap.push(make_pair(dist, i));
  }

  vector<pair<uint32_t, uint64_t> > sorted;
  heap.get_sorted(sorted);

  ret.clear();
  const float denom = query.bit_num();
  for (size_t i = 0; i < sorted.size(); ++i) {
    ret.push_back(make_pair(sorted[i].second, sorted[i].first / denom));
  }
}

}  // namespace nearest_neighbor
}  // namespace jubatus
