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
    const common::sfv_t& query,
    const vector<pair<string, float> >& ids) {
  similar_relation_[query] = ids;
}

void recommender_mock_storage::set_neighbor_items(
    const common::sfv_t& query,
    const vector<pair<string, float> >& ids) {
  neighbor_relation_[query] = ids;
}

void recommender_mock_storage::similar_items_similarity(
    const common::sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  get_relation(query, similar_relation_, ret_num, ids);
}

void recommender_mock_storage::neighbor_items_distance(
    const common::sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  get_relation(query, neighbor_relation_, ret_num, ids);
}

void recommender_mock_storage::update(const common::sfv_t& from,
                                      const common::sfv_t& to) {
  update_relation_key(from, to, similar_relation_);
  update_relation_key(from, to, neighbor_relation_);
}

void recommender_mock_storage::remove(const common::sfv_t& query) {
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

bool recommender_mock_storage::save(std::ostream& os) {
  pfi::data::serialization::binary_oarchive bo(os);
  bo << *this;
  return true;
}

bool recommender_mock_storage::load(std::istream& is) {
  pfi::data::serialization::binary_iarchive bi(is);
  bi >> *this;
  return true;
}

void recommender_mock_storage::pack(
    msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack(*this);
}

void recommender_mock_storage::unpack(msgpack::object o) {
  o.convert(this);
}

void recommender_mock_storage::get_diff(recommender_mock_storage& diff) const {
  diff = *this;
}

void recommender_mock_storage::set_mixed_and_clear_diff(
    const recommender_mock_storage& mixed_diff) {
  *this = mixed_diff;
}

void recommender_mock_storage::mix(
    const recommender_mock_storage& lhs,
    recommender_mock_storage& rhs) const {
  mix_relation(lhs.similar_relation_, rhs.similar_relation_);
  mix_relation(lhs.neighbor_relation_, rhs.neighbor_relation_);
}

// private

// static
void recommender_mock_storage::get_relation(
    const common::sfv_t& query,
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
    const common::sfv_t& from,
    const common::sfv_t& to,
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
