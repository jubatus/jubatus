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

#include <sstream>
#include <pficommon/data/serialization/unordered_map.h>
#include "lsh_index_storage.hpp"

using namespace std;

namespace jubatus {
namespace storage {

namespace {

struct equal_first {
  bool operator()(const pair<string, float>& l, const pair<string, float>& r) const {
    return l.first == r.first;
  }
};

struct greater_second {
  bool operator()(const pair<string, float>& l, const pair<string, float>& r) const {
    return l.second > r.second;
  }
};

lsh_vector nth_lsh_vector(const lsh_vector& lv, size_t num_tables, size_t n) {
  const size_t cut_len = lv.size() / num_tables;
  return lv.cut(n * cut_len, cut_len);
}

size_t calc_hamming_similarity(const lsh_vector& l, const lsh_vector& r) {
  if (l.size() != r.size()) return 0;

  size_t count = 0;
  for (size_t i = 0; i < l.size(); ++i) {
    if (l.get(i) == r.get(i)) {
      ++count;
    }
  }

  return count;
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

lsh_index_storage::lsh_index_storage(size_t num_tables)
    : lsh_tables_(num_tables) {
}

lsh_index_storage::~lsh_index_storage() {
}

void lsh_index_storage::set_row(const string& row, const lsh_vector& lv) {
  lsh_master_table_t::iterator it = remove_row_and_get_iterator(row);
  if (it == master_table_diff_.end()) {
    master_table_diff_.insert(make_pair(row, lv));
  } else {
    it->second = lv;
  }

  add_index(row, lv);
}

void lsh_index_storage::get_row(const string& row, lsh_vector& lv) const {
  lsh_master_table_t::const_iterator it = master_table_diff_.find(row);
  if (it == master_table_diff_.end()) {
    it = master_table_.find(row);
    if (it == master_table_.end() || it->second.size() == 0) {
      lv = lsh_vector();
      return;
    }
  }
  lv = it->second;
}

void lsh_index_storage::remove_row(const string& row) {
  remove_row_and_get_iterator(row);
}

void lsh_index_storage::clear() {
  master_table_.clear();
  master_table_diff_.clear();
  for (size_t i = 0; i < lsh_tables_.size(); ++i) {
    lsh_tables_[i].clear();
  }
}

void lsh_index_storage::get_all_row_ids(vector<string>& ids) const {
  vector<string> ret;

  if (!lsh_tables_.empty()) {
    const lsh_table_t& tbl = lsh_tables_.front();
    ret.reserve(tbl.size());

    for (lsh_table_t::const_iterator it = tbl.begin(); it != tbl.end(); ++it) {
      ret.push_back(it->second);
    }
  }

  ids.swap(ret);
}

void lsh_index_storage::similar_row(const lsh_vector& lv,
                                    vector<pair<string, float> >& ids,
                                    uint64_t ret_num) const {
  vector<pair<string, float> > ret;

  for (size_t i = 0; i < lsh_tables_.size(); ++i) {
    const lsh_vector query = nth_lsh_vector(lv, lsh_tables_.size(), i);
    const pair<lsh_table_t::const_iterator, lsh_table_t::const_iterator>
        range = lsh_tables_[i].equal_range(query);
    for (lsh_table_t::const_iterator it = range.first; it != range.second; ++it) {
      const float score = get_score(it->second, lv);
      ret.push_back(make_pair(it->second, score));
    }
  }

  sort(ret.begin(), ret.end());
  ret.erase(unique(ret.begin(), ret.end(), equal_first()), ret.end());

  sort(ret.begin(), ret.end(), greater_second());
  if (ret_num < ret.size()) {
    ret.resize(ret_num);
  }

  ids.swap(ret);
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
    if (it->second.size() == 0) {
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

lsh_master_table_t::iterator
lsh_index_storage::remove_row_and_get_iterator(const string& row) {
  lsh_master_table_t::iterator it = master_table_diff_.find(row);

  if (it == master_table_diff_.end()) {
    it = master_table_.find(row);
    if (it == master_table_.end()) {
      return master_table_diff_.end();
    }
  }

  for (size_t i = 0; i < lsh_tables_.size(); ++i) {
    const lsh_vector query = nth_lsh_vector(it->second, lsh_tables_.size(), i);
    const pair<lsh_table_t::iterator, lsh_table_t::iterator> range =
        lsh_tables_[i].equal_range(query);
    for (lsh_table_t::iterator it = range.first; it != range.second; ++it) {
      if (it->second == row) {
        lsh_tables_[i].erase(it);
        break;
      }
    }
  }

  master_table_diff_[row] = lsh_vector();

  return it;
}

void lsh_index_storage::add_index(const string& row, const lsh_vector& lv) {
  for (size_t i = 0; i < lsh_tables_.size(); ++i ){
    const lsh_vector query = nth_lsh_vector(lv, lsh_tables_.size(), i);
    const pair<lsh_table_t::iterator, lsh_table_t::iterator> range =
        lsh_tables_[i].equal_range(query);

    lsh_table_t::iterator it = range.first;
    for (; it != range.second; ++it) {
      if (it->second == row) {
        break;
      }
    }
    if (it == range.second) {
      lsh_tables_[i].insert(make_pair(query, row));
    }
  }
}

float lsh_index_storage::get_score(const string& row, const lsh_vector& lv) const {
  if (lv.size() == 0) return 0;

  lsh_master_table_t::const_iterator it = master_table_diff_.find(row);
  if (it == master_table_diff_.end()) {
    it = master_table_.find(row);
    if (it == master_table_.end()) {
      return 0;
    }
  }

  const lsh_vector& target = it->second;
  const size_t sim = calc_hamming_similarity(target, lv);

  return static_cast<float>(sim) / lv.size();
}

void lsh_index_storage::remove_model_row(const std::string& row) {
  lsh_master_table_t::iterator it = master_table_.find(row);
  if (it != master_table_.end()) {
    master_table_.erase(it);
  } else {
    it = master_table_diff_.find(row);
    if (it == master_table_diff_.end()) {
      return;
    }
  }

  for (size_t i = 0; i < lsh_tables_.size(); ++i ){
    const lsh_vector query = nth_lsh_vector(it->second, lsh_tables_.size(), i);
    const pair<lsh_table_t::iterator, lsh_table_t::iterator> range =
        lsh_tables_[i].equal_range(query);
    for (lsh_table_t::iterator it = range.first; it != range.second; ++it) {
      if (it->second == row) {
        lsh_tables_[i].erase(it);
        break;
      }
    }
  }
}

void lsh_index_storage::set_mixed_row(const std::string& row, const lsh_vector& lv) {
  // We assume that hash values of lv does not change at any time.
  master_table_[row] = lv;
  for (size_t i = 0; i < lsh_tables_.size(); ++i) {
    const lsh_vector query = nth_lsh_vector(lv, lsh_tables_.size(), i);
    lsh_tables_[i].insert(make_pair(query, row));
  }
}

}
}
