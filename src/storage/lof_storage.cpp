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

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "../common/config_util.hpp"
#include "lof_storage.hpp"

using namespace std;
using pfi::data::serialization::binary_iarchive;
using pfi::data::serialization::binary_oarchive;
using pfi::data::unordered_map;
using pfi::data::unordered_set;

namespace jubatus {
namespace storage {

namespace {

const uint32_t DEFAULT_NEIGHBOR_NUM = 10;
const uint32_t DEFAULT_REVERSE_NN_NUM = 30;

template<typename T>
void deserialize_diff(const string& diff, T& t) {
  istringstream iss(diff);
  binary_iarchive ia(iss);
  ia >> t;
}

template<typename T>
string serialize_diff(T& t) {
  ostringstream oss;
  binary_oarchive oa(oss);
  oa << t;
  return oss.str();
}

}

lof_storage::lof_storage()
    : neighbor_num_(DEFAULT_NEIGHBOR_NUM),
      reverse_nn_num_(DEFAULT_REVERSE_NN_NUM) {
}

lof_storage::lof_storage(const map<string, string>& config)
    : neighbor_num_(get_param(config, "lof:neighbor_num", DEFAULT_NEIGHBOR_NUM)),
      reverse_nn_num_(get_param(config, "lof:reverse_nn_num", DEFAULT_REVERSE_NN_NUM)),
      nn_engine_(config) {
}

lof_storage::~lof_storage() {
}

float lof_storage::collect_lrds(const sfv_t& query,
                                unordered_map<string, float>& neighbor_lrd) const {
  vector<pair<string, float> > neighbors;
  nn_engine_.neighbor_row(query, neighbors, neighbor_num_);

  // collect lrd values of the nearest neighbors
  neighbor_lrd.clear();
  for (size_t i = 0; i < neighbors.size(); ++i) {
    neighbor_lrd[neighbors[i].first] = get_lrd(neighbors[i].first);
  }

  // return lrd of the query
  float sum_reachability = 0;
  for (size_t i = 0; i < neighbors.size(); ++i) {
    sum_reachability += max(neighbors[i].second, get_kdist(neighbors[i].first));
  }
  return neighbors.size() / sum_reachability;
}

void lof_storage::remove_row(const string& row) {
  lof_table_.erase(row);
  nn_engine_.clear_row(row);
}

void lof_storage::clear() {
  lof_table_.clear();
  nn_engine_.clear();
}

void lof_storage::get_all_row_ids(vector<string>& ids) const {
  nn_engine_.get_all_row_ids(ids);
}

void lof_storage::update_row(const string& row, const sfv_t& diff) {
  sfv_t query;
  nn_engine_.decode_row(row, query);

  if (!query.empty()) {
    push_neighbors_to_update_queue(query);
  }

  nn_engine_.update_row(row, diff);
  push_neighbors_to_update_queue(row);
}

string lof_storage::name() const {
  return "lof_storage";
}

float lof_storage::get_kdist(const string& row) const {
  lof_table_t::const_iterator it = lof_table_.find(row);
  if (it == lof_table_.end()) {
    throw runtime_error("lof_storage::get_kdist: row \"" + row + "\" does not exist");
  }
  return it->second.kdist;
}

float lof_storage::get_lrd(const string& row) const {
  lof_table_t::const_iterator it = lof_table_.find(row);
  if (it == lof_table_.end()) {
    throw runtime_error("lof_storage::get_lrd: row \"" + row + "\" does not exist");
  }
  return it->second.lrd;
}

void lof_storage::update_all() {
  update_all_kdist();
  update_all_lrd();
  update_queue_.clear();
}

bool lof_storage::save(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool lof_storage::load(istream& is) {
  binary_iarchive ia(is);
  ia >> *this;
  return true;
}

void lof_storage::get_diff(string& diff) const {
  diff = serialize_diff(const_cast<lof_table_t&>(lof_table_diff_));
}

void lof_storage::set_mixed_and_clear_diff(const string& mixed_diff) {
  deserialize_diff(mixed_diff, lof_table_diff_);

  for (lof_table_t::const_iterator it = lof_table_diff_.begin();
       it != lof_table_diff_.end(); ++it) {
    lof_table_[it->first] = it->second;
  }
  lof_table_diff_.clear();

  // kdists for all items must be updated before updating lrds,
  // since lrd of some item depends on kdists of its nearest neighbors.

  for (unordered_set<string>::const_iterator it = update_queue_.begin();
       it != update_queue_.end(); ++it) {
    update_kdist(*it);
  }

  for (unordered_set<string>::const_iterator it = update_queue_.begin();
       it != update_queue_.end(); ++it) {
    update_lrd(*it);
  }

  update_queue_.clear();
}

void lof_storage::mix(const string& lhs, string& rhs) const {
  lof_table_t diff, mixed;
  deserialize_diff(lhs, diff);
  deserialize_diff(rhs, mixed);

  for (lof_table_t::const_iterator it = diff.begin(); it != diff.end(); ++it) {
    mixed[it->first] = it->second;
  }

  rhs = serialize_diff(mixed);
}

// private

void lof_storage::update_all_kdist() {
  for (unordered_set<string>::const_iterator it = update_queue_.begin();
       it != update_queue_.end(); ++it) {
    update_kdist(*it);
  }
}

void lof_storage::update_all_lrd() {
  for (unordered_set<string>::const_iterator it = update_queue_.begin();
       it != update_queue_.end(); ++it) {
    update_lrd(*it);
  }
}

void lof_storage::update_kdist(const string& row) {
  vector<pair<string, float> > neighbors;
  nn_engine_.neighbor_row(row, neighbors, neighbor_num_);
  lof_table_diff_[row].kdist = neighbors.back().second;
}

void lof_storage::update_lrd(const string& row) {
  vector<pair<string, float> > neighbors;
  nn_engine_.neighbor_row(row, neighbors, neighbor_num_);

  float sum_reachability = 0;
  for (size_t i = 0; i < neighbors.size(); ++i) {
    sum_reachability += max(neighbors[i].second, get_kdist(neighbors[i].first));
  }

  lof_table_diff_[row].lrd = neighbors.size() / sum_reachability;
}

void lof_storage::push_neighbors_to_update_queue(const string& row) {
  vector<pair<string, float> > neighbors;
  nn_engine_.neighbor_row(row, neighbors, reverse_nn_num_);

  for (size_t i = 0; i < neighbors.size(); ++i) {
    update_queue_.insert(neighbors[i].first);
  }
  update_queue_.insert(row);
}

void lof_storage::push_neighbors_to_update_queue(const sfv_t& query) {
  vector<pair<string, float> > neighbors;
  nn_engine_.neighbor_row(query, neighbors, reverse_nn_num_);

  for (size_t i = 0; i < neighbors.size(); ++i) {
    update_queue_.insert(neighbors[i].first);
  }
}

}
}
