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

#include "lsh.hpp"

#include <cmath>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "../common/exception.hpp"
#include "../common/hash.hpp"
#include "lsh_util.hpp"

using std::pair;
using std::string;
using std::vector;
using jubatus::core::storage::bit_vector;

namespace jubatus {
namespace core {
namespace recommender {

static const uint64_t DEFAULT_BASE_NUM = 64;  // should be in config

lsh::config::config()
    : bit_num(DEFAULT_BASE_NUM) {
}

lsh::lsh(uint64_t base_num)
    : base_num_(base_num) {
  if (base_num == 0) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("base_num == 0"));
  }
}

lsh::lsh(const config& config)
    : base_num_(config.bit_num) {
}

lsh::lsh()
    : base_num_(DEFAULT_BASE_NUM) {
}

lsh::~lsh() {
}

void lsh::similar_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  if (ret_num == 0) {
    return;
  }

  bit_vector query_bv;
  calc_lsh_values(query, query_bv);
  row2lshvals_.similar_row(query_bv, ids, ret_num);
}

void lsh::neighbor_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = 1 - ids[i].second;
  }
}

void lsh::clear() {
  orig_.clear();
  pfi::data::unordered_map<std::string, std::vector<float> >()
    .swap(column2baseval_);
  row2lshvals_.clear();
}

void lsh::clear_row(const string& id) {
  orig_.remove_row(id);
  row2lshvals_.remove_row(id);
}

void lsh::calc_lsh_values(const sfv_t& sfv, bit_vector& bv) const {
  const_cast<lsh*>(this)->generate_column_bases(sfv);

  vector<float> lsh_vals;
  prod_invert_and_vector(column2baseval_, sfv, base_num_, lsh_vals);
  set_bit_vector(lsh_vals, bv);
}

void lsh::generate_column_bases(const sfv_t& sfv) {
  for (size_t i = 0; i < sfv.size(); ++i) {
    generate_column_base(sfv[i].first);
  }
}

void lsh::generate_column_base(const string& column) {
  if (column2baseval_.count(column) != 0) {
    return;
  }
  const uint32_t seed = hash_util::calc_string_hash(column);
  generate_random_vector(base_num_, seed, column2baseval_[column]);
}

void lsh::update_row(const string& id, const sfv_diff_t& diff) {
  generate_column_bases(diff);
  orig_.set_row(id, diff);
  sfv_t row;
  orig_.get_row(id, row);
  bit_vector bv;
  calc_lsh_values(row, bv);
  row2lshvals_.set_row(id, bv);
}

void lsh::get_all_row_ids(std::vector<std::string>& ids) const {
  row2lshvals_.get_all_row_ids(ids);
}

string lsh::type() const {
  return string("lsh");
}

bool lsh::save_impl(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << column2baseval_;
  oa << row2lshvals_;
  return true;
}

bool lsh::load_impl(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> column2baseval_;
  ia >> row2lshvals_;
  return true;
}

core::storage::recommender_storage_base* lsh::get_storage() {
  return &row2lshvals_;
}

const core::storage::recommender_storage_base* lsh::get_const_storage() const {
  return &row2lshvals_;
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
