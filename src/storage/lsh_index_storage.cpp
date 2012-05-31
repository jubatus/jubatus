// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <cmath>
#include <sstream>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include <pficommon/math/random.h>
#include "lsh_index_storage.hpp"
#include "lsh_util.hpp"

using namespace std;
using pfi::data::unordered_map;
using pfi::data::unordered_set;
using pfi::math::random::mtrand;

namespace jubatus {
namespace storage {

namespace {

struct greater_second {
  bool operator()(const pair<string, float>& l, const pair<string, float>& r) const {
    return l.second > r.second;
  }
};

uint64_t hash_lv(const lsh_vector& lv) {
  uint64_t hash = 14695981039346656037LLU;
  for (size_t i = 0; i < lv.size(); ++i) {
    for (int j = 0; j < 32; j += 8) {
      hash *= 1099511628211LLU;
      hash ^= ((uint32_t)lv.get(i) >> j) & 0xff;
    }
  }
  return hash;
}

void initialize_shift(uint32_t seed, vector<float>& shift) {
  mtrand rnd(seed);
  for (size_t i = 0; i < shift.size(); ++i) {
    shift[i] = rnd.next_double();
  }
}

vector<float> shift_hash(const vector<float>& hash, const vector<float>& shift) {
  vector<float> shifted(hash);
  for (size_t i = 0; i < shifted.size(); ++i) {
    shifted[i] += shift[i];
  }
  return shifted;
}

bit_vector binarize(const vector<float>& hash) {
  bit_vector bv;
  bv.resize_and_clear(hash.size());
  for (size_t i = 0; i < hash.size(); ++i) {
    if (hash[i] > 0) {
      bv.set_bit(i);
    }
  }
  return bv;
}

float calc_euclidean_distance(const lsh_entry& entry, const bit_vector& bv, float norm) {
  const uint64_t hamm = bv.calc_hamming_similarity(entry.simhash_bv);
  const float angle = (1 - float(hamm) / bv.bit_num()) * M_PI;
  const float dot = entry.norm * norm * cos(angle);
  return sqrt(norm * norm + entry.norm * entry.norm - 2 * dot);
}

lsh_master_table_t extract_diff(const string& serialized) {
  lsh_master_table_t diff;
  istringstream iss(serialized);
  pfi::data::serialization::binary_iarchive bi(iss);
  bi >> diff;
  return diff;
}

string serialize_diff(const lsh_master_table_t& table) {
  ostringstream oss;
  pfi::data::serialization::binary_oarchive bo(oss);
  bo << const_cast<lsh_master_table_t&>(table);
  return oss.str();  // TODO remove redundant copy
}

}

lsh_index_storage::lsh_index_storage() {
}

lsh_index_storage::lsh_index_storage(size_t lsh_num, size_t table_num, uint32_t seed)
    : shift_(lsh_num * table_num),
      table_num_(table_num) {
  initialize_shift(seed, shift_);
}

lsh_index_storage::~lsh_index_storage() {
}

void lsh_index_storage::set_row(const string& row, const vector<float>& hash, float norm) {
  remove_row(row);
  lsh_entry& entry = master_table_diff_[row];
  make_entry(hash, norm, entry);
  add_index(row, entry);
}

void lsh_index_storage::remove_row(const string& row) {
  const uint64_t row_id = key_manager_.get_id_const(row);
  if (row_id == key_manager::NOTFOUND) {
    return;
  }

  lsh_master_table_t::iterator entry_it = master_table_diff_.find(row);
  if (entry_it == master_table_diff_.end()) {
    entry_it = master_table_.end();
    master_table_diff_[row] = lsh_entry();
  }
  lsh_entry& entry = entry_it->second;

  for (size_t i = 0; i < entry.lsh_hash.size(); ++i) {
    pair<lsh_table_t::iterator, lsh_table_t::iterator> range =
        lsh_table_.equal_range(entry.lsh_hash[i]);
    for (lsh_table_t::iterator it = range.first; it != range.second; ++it) {
      if (it->second == row_id) {
        lsh_table_.erase(it);
        break;
      }
    }
  }
  entry = lsh_entry();
}

void lsh_index_storage::clear() {
  master_table_.clear();
  master_table_diff_.clear();
  lsh_table_.clear();
  key_manager_.clear();
}

void lsh_index_storage::get_all_row_ids(vector<string>& ids) const {
  const size_t size_upper_bound = master_table_.size() + master_table_diff_.size();

  unordered_set<std::string> id_set;
  // equivalent to id_set.reserve(size_upper_bound) in C++11
  id_set.rehash(ceil(size_upper_bound / id_set.max_load_factor()));

  for (lsh_master_table_t::const_iterator it = master_table_.begin();
       it != master_table_.end(); ++it) {
    if (!it->second.lsh_hash.empty()) {
      id_set.insert(it->first);
    }
  }
  for (lsh_master_table_t::const_iterator it = master_table_diff_.begin();
       it != master_table_diff_.end(); ++it) {
    if (!it->second.lsh_hash.empty()) {
      id_set.insert(it->first);
    }
  }

  vector<string> ret(id_set.size());
  copy(id_set.begin(), id_set.end(), ret.begin());
  ids.swap(ret);
}

void lsh_index_storage::similar_row(const vector<float>& hash,
                                    float norm,
                                    uint64_t probe_num,
                                    uint64_t ret_num,
                                    vector<pair<string, float> >& ids) const {
  const vector<float> shifted = shift_hash(hash, shift_);
  const bit_vector bv = binarize(hash);

  lsh_probe_generator gen(shifted, table_num_);
  unordered_set<uint64_t> cands;

  for (uint64_t i = 0; i < table_num_; ++i) {
    lsh_vector key = gen.base(i);
    key.push_back(i);
    if (retrieve_hit_rows(key, ret_num, cands)) {
      get_sorted_similar_rows(cands, bv, norm, ret_num, ids);
      return;
    }
  }

  gen.init();
  for (uint64_t i = 0; i < probe_num; ++i) {
    pair<size_t, lsh_vector> p = gen.get_next_table_and_vector();
    p.second.push_back(p.first);
    if (retrieve_hit_rows(p.second, ret_num, cands)) {
      break;
    }
  }
  get_sorted_similar_rows(cands, bv, norm, ret_num, ids);
}

string lsh_index_storage::name() const {
  return "lsh_index_storage";
}

bool lsh_index_storage::save(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool lsh_index_storage::load(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

void lsh_index_storage::get_diff(string& diff) const {
  diff = serialize_diff(master_table_diff_);
}

void lsh_index_storage::set_mixed_and_clear_diff(const string& mixed_diff) {
  lsh_master_table_t diff = extract_diff(mixed_diff);

  for (lsh_master_table_t::const_iterator it = diff.begin(); it != diff.end(); ++it) {
    if (it->second.lsh_hash.empty()) {
      remove_model_row(it->first);
    } else {
      set_mixed_row(it->first, it->second);
    }
  }

  master_table_diff_.clear();
}

void lsh_index_storage::mix(const string& lhs, string& rhs) const {
  const lsh_master_table_t diff_l = extract_diff(lhs);
  lsh_master_table_t diff_r = extract_diff(rhs);

  for (lsh_master_table_t::const_iterator it = diff_l.begin(); it != diff_l.end(); ++it) {
    diff_r[it->first] = it->second;
  }

  rhs = serialize_diff(diff_r);
}

// private

vector<float> lsh_index_storage::make_entry(const vector<float>& hash,
                                            float norm,
                                            lsh_entry& entry) const {
  const vector<float> shifted = shift_hash(hash, shift_);
  lsh_probe_generator gen(shifted, table_num_);

  entry.lsh_hash.resize(table_num_);
  for (uint64_t i = 0; i < table_num_; ++i) {
    lsh_vector key = gen.base(i);
    key.push_back(i);
    entry.lsh_hash[i] = hash_lv(key);
  }

  entry.simhash_bv = binarize(hash);
  entry.norm = norm;

  return shifted;
}

void lsh_index_storage::add_index(const string& row, const lsh_entry& entry) {
  const uint64_t row_id = key_manager_.get_id(row);
  for (size_t i = 0; i < entry.lsh_hash.size(); ++i) {
    lsh_table_.insert(make_pair(entry.lsh_hash[i], row_id));
  }
}

void lsh_index_storage::remove_model_row(const std::string& row) {
  const lsh_entry* entry = get_lsh_entry(row);
  if (!entry) {
    return;
  }

  const uint64_t row_id = key_manager_.get_id_const(row);
  for (size_t i = 0; i < entry->lsh_hash.size(); ++i) {
    const pair<lsh_table_t::iterator, lsh_table_t::iterator>
        range = lsh_table_.equal_range(entry->lsh_hash[i]);
    for (lsh_table_t::iterator it = range.first; it != range.second; ++it) {
      if (it->second == row_id) {
        lsh_table_.erase(it);
        break;
      }
    }
  }
}

void lsh_index_storage::set_mixed_row(const string& row, const lsh_entry& entry) {
  const uint64_t row_id = key_manager_.get_id(row);
  master_table_[row] = entry;
  for (size_t i = 0; i < entry.lsh_hash.size(); ++i) {
    lsh_table_.insert(make_pair(entry.lsh_hash[i], row_id));
  }
}

bool lsh_index_storage::retrieve_hit_rows(const lsh_vector& key,
                                          size_t ret_num,
                                          unordered_set<uint64_t>& cands) const {
  const size_t hash = hash_lv(key);
  const pair<lsh_table_t::const_iterator, lsh_table_t::const_iterator>
      range = lsh_table_.equal_range(hash);

  for (lsh_table_t::const_iterator it = range.first; it != range.second; ++it) {
    cands.insert(it->second);
  }
  return cands.size() >= max(ret_num, table_num_ * 3);
}

void lsh_index_storage::get_sorted_similar_rows(const unordered_set<uint64_t>& cands,
                                                const bit_vector& query_simhash,
                                                float query_norm,
                                                uint64_t ret_num,
                                                vector<pair<string, float> >& ids) const {
  ids.clear();
  ids.reserve(cands.size());
  for (unordered_set<uint64_t>::const_iterator it = cands.begin();
       it != cands.end(); ++it) {
    const string& row = key_manager_.get_key(*it);
    const lsh_entry* entry = get_lsh_entry(row);
    if (!entry) {
      continue;
    }
    const float dist = calc_euclidean_distance(*entry, query_simhash, query_norm);
    ids.push_back(make_pair(row, -dist));
  }
  sort(ids.begin(), ids.end(), greater_second());

  if (ids.size() > ret_num) {
    ids.resize(ret_num);
  }
}

const lsh_entry* lsh_index_storage::get_lsh_entry(const string& row) const {
  lsh_master_table_t::const_iterator it = master_table_diff_.find(row);
  if (it == master_table_diff_.end()) {
    it = master_table_.find(row);
    if (it == master_table_.end()) {
      return 0;
    }
  }
  return &it->second;
}

}
}
