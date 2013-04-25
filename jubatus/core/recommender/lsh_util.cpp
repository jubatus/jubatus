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

#include "lsh_util.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <pficommon/math/random.h>
#include "../storage/bit_vector.hpp"

namespace jubatus {
namespace core {
namespace recommender {

using std::string;
using std::vector;
using jubatus::core::storage::bit_vector;
using pfi::data::unordered_map;

void generate_random_vector(size_t dim, uint32_t seed, vector<float>& ret) {
  pfi::math::random::mtrand rnd(seed);
  vector<float> v(dim);
  for (uint64_t i = 0; i < dim; ++i) {
    v[i] = rnd.next_gaussian();
  }
  ret.swap(v);
}

void set_bit_vector(const std::vector<float>& vec, bit_vector& bit_vec) {
  bit_vector bv;
  bv.resize_and_clear(vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    if (vec[i] >= 0.f) {
      bv.set_bit(i);
    }
  }
  bit_vec.swap(bv);
}

void prod_invert_and_vector(
    const unordered_map<string, vector<float> >& matrix,
    const sfv_t& vec,
    size_t dim,
    vector<float>& ret) {
  vector<float> r(dim);
  for (size_t i = 0; i < vec.size(); ++i) {
    const string& column = vec[i].first;
    float val = vec[i].second;
    unordered_map<string, vector<float> >::const_iterator it =
        matrix.find(column);
    if (it == matrix.end()) {
      continue;
    }
    const vector<float>& v = it->second;

    // assert(v.size() == base_num_);
    for (size_t j = 0; j < v.size(); ++j) {
      r[j] += v[j] * val;
    }
  }

  ret.swap(r);
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
