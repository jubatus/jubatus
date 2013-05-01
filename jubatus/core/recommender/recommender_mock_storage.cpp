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

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "recommender_mock_storage.hpp"

using std::istringstream;
using std::ostringstream;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace recommender {

recommender_mock_storage::~recommender_mock_storage() {
}

void recommender_mock_storage::set_similar_items(
    const sfv_t& query,
    const vector<pair<string, float> >& ids) {
  similar_relation_[query] = ids;
}

void recommender_mock_storage::set_neighbor_items(
    const sfv_t& query,
    const vector<pair<string, float> >& ids) {
  neighbor_relation_[query] = ids;
}

void recommender_mock_storage::similar_items_similarity(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  get_relation(query, similar_relation_, ret_num, ids);
}

void recommender_mock_storage::neighbor_items_distance(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  get_relation(query, neighbor_relation_, ret_num, ids);
}

void recommender_mock_storage::update(const sfv_t& from, const sfv_t& to) {
  update_relation_key(from, to, similar_relation_);
  update_relation_key(from, to, neighbor_relation_);
}

void recommender_mock_storage::remove(const sfv_t& query) {
  similar_relation_.erase(query);
  neighbor_relation_.erase(query);
}

void recommender_mock_storage::clear() {
  similar_relation_.clear();
  neighbor_relation_.clear();
}

string recommender_mock_storage::name() const {
  return "recommender_mock_storage";
}

void recommender_mock_storage::get_diff(string& diff) const {
  ostringstream oss;
  pfi::data::serialization::binary_oarchive bo(oss);
  bo << const_cast<recommender_mock_storage&>(*this);
  diff = oss.str();
}

void recommender_mock_storage::set_mixed_and_clear_diff(
    const string& mixed_diff) {
  istringstream iss(mixed_diff);
  pfi::data::serialization::binary_iarchive bi(iss);
  bi >> *this;
}

void recommender_mock_storage::mix(const string& lhs, string& rhs) const {
  recommender_mock_storage lhs_model, rhs_model;
  lhs_model.set_mixed_and_clear_diff(lhs);
  rhs_model.set_mixed_and_clear_diff(rhs);

  mix_relation(lhs_model.similar_relation_, rhs_model.similar_relation_);
  mix_relation(lhs_model.neighbor_relation_, rhs_model.neighbor_relation_);

  rhs_model.get_diff(rhs);
}

// private

// static
void recommender_mock_storage::get_relation(
    const sfv_t& query,
    const relation_type& relmap,
    size_t ret_num,
    vector<pair<string, float> >& ids) {
  ids.clear();

  relation_type::const_iterator it = relmap.find(query);
  if (it != relmap.end()) {
    ids = it->second;
  }

  if (ids.size() > ret_num) {
    ids.resize(ret_num);
  }
}

// static
void recommender_mock_storage::update_relation_key(
    const sfv_t& from,
    const sfv_t& to,
    relation_type& relmap) {
  relation_type::iterator it = relmap.find(from);
  if (it != relmap.end()) {
    relation_type::mapped_type val;
    val.swap(it->second);
    relmap.erase(it);
    relmap[to].swap(val);
  } else {
    relmap[to];  // add default value
  }
}

// static
void recommender_mock_storage::mix_relation(
    const relation_type& from,
    relation_type& to) {
  for (relation_type::const_iterator it = from.begin(); it != from.end();
      ++it) {
    to[it->first] = it->second;
  }
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
