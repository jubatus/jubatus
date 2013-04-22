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

#include "bit_index_storage.hpp"
#include <functional>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include "fixed_size_heap.hpp"

using std::greater;
using std::istringstream;
using std::ostringstream;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

bit_index_storage::bit_index_storage() {
}

bit_index_storage::~bit_index_storage() {
}

void bit_index_storage::set_row(const string& row, const bit_vector& bv) {
  bitvals_diff_[row] = bv;
}

void bit_index_storage::get_row(const string& row, bit_vector& bv) const {
  {
    bit_table_t::const_iterator it = bitvals_diff_.find(row);
    if (it != bitvals_diff_.end()) {
      bv = it->second;
      return;
    }
  }
  {
    bit_table_t::const_iterator it = bitvals_.find(row);
    if (it != bitvals_.end()) {
      bv = it->second;
      return;
    }
  }
  bv = bit_vector();
}

void bit_index_storage::remove_row(const string& row) {
  bitvals_diff_[row] = bit_vector();
}

void bit_index_storage::clear() {
  bit_table_t().swap(bitvals_);
  bit_table_t().swap(bitvals_diff_);
}

void bit_index_storage::get_all_row_ids(std::vector<std::string>& ids) const {
  ids.clear();
  for (bit_table_t::const_iterator it = bitvals_.begin(); it != bitvals_.end();
      ++it) {
    ids.push_back(it->first);
  }
  for (bit_table_t::const_iterator it = bitvals_diff_.begin();
      it != bitvals_diff_.end(); ++it) {
    if (bitvals_.find(it->first) == bitvals_.end()) {
      ids.push_back(it->first);
    }
  }
}

void bit_index_storage::get_diff(string& diff) const {
  ostringstream os;
  {
    pfi::data::serialization::binary_oarchive bo(os);
    bo << const_cast<bit_table_t&>(bitvals_diff_);
  }
  diff = os.str();  // TODO(unknown) remove redudant copy
}

void bit_index_storage::set_mixed_and_clear_diff(const string& mixed_diff_str) {
  istringstream is(mixed_diff_str);
  pfi::data::serialization::binary_iarchive bi(is);
  bit_table_t mixed_diff;
  bi >> mixed_diff;
  for (bit_table_t::const_iterator it = mixed_diff.begin();
      it != mixed_diff.end(); ++it) {
    bitvals_[it->first] = it->second;
  }
  bitvals_diff_.clear();
}

void bit_index_storage::mix(const string& lhs, string& rhs) const {
  bit_table_t lhs_diff;
  {
    istringstream is(lhs);
    pfi::data::serialization::binary_iarchive bi(is);
    bi >> lhs_diff;
  }
  bit_table_t rhs_diff;
  {
    istringstream is(rhs);
    pfi::data::serialization::binary_iarchive bi(is);
    bi >> rhs_diff;
  }

  for (bit_table_t::const_iterator it = lhs_diff.begin(); it != lhs_diff.end();
      ++it) {
    rhs_diff[it->first] = it->second;
  }

  ostringstream os;
  {
    pfi::data::serialization::binary_oarchive bo(os);
    bo << rhs_diff;
  }
  rhs = os.str();  // TODO(unknown) remove redudant copy
}

typedef fixed_size_heap<pair<uint64_t, string>,
    greater<pair<uint64_t, string> > > heap_type;

static void similar_row_one(
    const bit_vector& x,
    const pair<string, bit_vector>& y,
    heap_type& heap) {
  uint64_t match_num = x.calc_hamming_similarity(y.second);
  heap.push(make_pair(match_num, y.first));
}

void bit_index_storage::similar_row(
    const bit_vector& bv,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  ids.clear();
  uint64_t bit_num = bv.bit_num();
  if (bit_num == 0) {
    return;
  }

  heap_type heap(ret_num);

  for (bit_table_t::const_iterator it = bitvals_diff_.begin();
      it != bitvals_diff_.end(); ++it) {
    similar_row_one(bv, *it, heap);
  }
  for (bit_table_t::const_iterator it = bitvals_.begin(); it != bitvals_.end();
      ++it) {
    if (bitvals_diff_.find(it->first) != bitvals_diff_.end()) {
      continue;
    }
    similar_row_one(bv, *it, heap);
  }

  vector<pair<uint64_t, string> > scores;
  heap.get_sorted(scores);
  for (size_t i = 0; i < scores.size() && i < ret_num; ++i) {
    ids.push_back(make_pair(scores[i].second,
                            static_cast<float>(scores[i].first) / bit_num));
  }
}

bool bit_index_storage::save(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool bit_index_storage::load(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

string bit_index_storage::name() const {
  return string("bit_index_storage");
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
