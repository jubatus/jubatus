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

#include <vector>

#include "lsh_function.hpp"

#include <pficommon/math/random.h>
#include "../common/hash.hpp"

using std::vector;
using jubatus::table::bit_vector;

namespace jubatus {
namespace nearest_neighbor {

vector<float> random_projection(const sfv_t& sfv, uint32_t hash_num) {
  vector<float> proj(hash_num);
  for (size_t i = 0; i < sfv.size(); ++i) {
    const uint32_t seed = hash_util::calc_string_hash(sfv[i].first);
    pfi::math::random::mtrand rnd(seed);
    for (uint32_t j = 0; j < hash_num; ++j) {
      proj[j] += sfv[i].second * rnd.next_gaussian();
    }
  }
  return proj;
}

bit_vector binarize(const vector<float>& proj) {
  bit_vector bv(proj.size());
  for (size_t i = 0; i < proj.size(); ++i) {
    if (proj[i] > 0) {
      bv.set_bit(i);
    }
  }
  return bv;
}

bit_vector cosine_lsh(const sfv_t& sfv, uint32_t hash_num) {
  return binarize(random_projection(sfv, hash_num));
}

}  // namespace nearest_neighbor
}  // namespace jubatus
