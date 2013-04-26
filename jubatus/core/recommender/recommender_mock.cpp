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

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include "recommender_mock.hpp"

using std::istream;
using std::ostream;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace recommender {

recommender_mock::recommender_mock() {
}

recommender_mock::~recommender_mock() {
}

void recommender_mock::set_similar_relation(
    const sfv_t& query,
    const vector<pair<string, float> >& ids) {
  storage_.set_similar_items(query, ids);
}

void recommender_mock::set_similar_relation(
    const string& id,
    const vector<pair<string, float> >& ids) {
  sfv_t query;
  decode_row(id, query);
  set_similar_relation(query, ids);
}

void recommender_mock::set_neighbor_relation(
    const sfv_t& query,
    const vector<pair<string, float> >& ids) {
  storage_.set_neighbor_items(query, ids);
}

void recommender_mock::set_neighbor_relation(
    const string& id,
    const vector<pair<string, float> >& ids) {
  sfv_t query;
  decode_row(id, query);
  set_neighbor_relation(query, ids);
}

void recommender_mock::similar_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  storage_.similar_items_similarity(query, ids, ret_num);
}

void recommender_mock::neighbor_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  storage_.neighbor_items_distance(query, ids, ret_num);
}

void recommender_mock::clear() {
  storage_.clear();
  orig_.clear();
}

void recommender_mock::clear_row(const string& id) {
  sfv_t sfv;
  decode_row(id, sfv);
  storage_.remove(sfv);

  orig_.remove_row(id);
}

void recommender_mock::update_row(const string& id, const sfv_diff_t& diff) {
  sfv_t old_sfv;
  orig_.get_row(id, old_sfv);

  orig_.set_row(id, diff);
  sfv_t new_sfv;
  orig_.get_row(id, new_sfv);

  storage_.update(old_sfv, new_sfv);
}

void recommender_mock::get_all_row_ids(vector<string>& ids) const {
  orig_.get_all_row_ids(ids);
}

string recommender_mock::type() const {
  return "recommender_mock";
}

core::storage::recommender_storage_base* recommender_mock::get_storage() {
  return &storage_;
}

const core::storage::recommender_storage_base*
    recommender_mock::get_const_storage()
    const {
  return &storage_;
}

bool recommender_mock::save_impl(ostream& os) {
  pfi::data::serialization::binary_oarchive bo(os);
  bo << *this;
  return true;
}

bool recommender_mock::load_impl(istream& is) {
  pfi::data::serialization::binary_iarchive bi(is);
  bi >> *this;
  return true;
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
