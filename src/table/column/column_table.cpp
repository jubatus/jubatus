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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <cstring>

#include "column_table.hpp"

namespace jubatus {
namespace table {

void column_table::init(const std::vector<column_type>& schema) {
  pfi::concurrent::scoped_lock lk(wlock(table_lock_));
  /* defining tuple */
  if (!columns_.empty()) {
    throw storage_exception(
      "Storage's schemas cannot be over written. Clear schemas before init()");
  }
  for (std::vector<column_type>::const_iterator it = schema.begin();
      it != schema.end();
      ++it) {
    columns_.push_back(detail::abstruct_column(*it));
  }
}

void column_table::clear() {
  pfi::concurrent::scoped_lock lk(wlock(table_lock_));
  // it keeps schema
  keys_.clear();
  versions_.clear();
  for (size_t i = 0; i < columns_.size(); ++i) {
    columns_[i].clear();
  }
  tuples_ = 0;
  clock_ = 0;
  index_.clear();
}

std::pair<bool, uint64_t> column_table::exact_match(
    const std::string& prefix) const {
  pfi::concurrent::scoped_lock lk(pfi::concurrent::rlock(table_lock_));
  index_table::const_iterator it = index_.find(prefix);
  if (it == index_.end()) {
    return std::make_pair(false, 0LLU);
  } else {
    return std::make_pair(true, it->second);
  }
}

uint8_column column_table::get_uint8_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::uint8_type));
  return columns_[column_id].as_uint8_column();
}
const_uint8_column column_table::get_uint8_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::uint8_type));
  return columns_[column_id].as_uint8_column();
}
uint16_column column_table::get_uint16_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::uint16_type));
  return columns_[column_id].as_uint16_column();
}
const_uint16_column column_table::get_uint16_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::uint16_type));
  return columns_[column_id].as_uint16_column();
}

uint32_column column_table::get_uint32_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::uint32_type));
  return columns_[column_id].as_uint32_column();
}
const_uint32_column column_table::get_uint32_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::uint32_type));
  return columns_[column_id].as_uint32_column();
}
uint64_column column_table::get_uint64_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::uint64_type));
  return columns_[column_id].as_uint64_column();
}
const_uint64_column column_table::get_uint64_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::uint64_type));
  return columns_[column_id].as_uint64_column();
}
int8_column column_table::get_int8_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::int8_type));
  return columns_[column_id].as_int8_column();
}
const_int8_column column_table::get_int8_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::int8_type));
  return columns_[column_id].as_int8_column();
}
int16_column column_table::get_int16_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::int16_type));
  return columns_[column_id].as_int16_column();
}
const_int16_column column_table::get_int16_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::int16_type));
  return columns_[column_id].as_int16_column();
}
int32_column column_table::get_int32_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::int32_type));
  return columns_[column_id].as_int32_column();
}
const_int32_column column_table::get_int32_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::int32_type));
  return columns_[column_id].as_int32_column();
}
int64_column column_table::get_int64_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::uint64_type));
  return columns_[column_id].as_int64_column();
}
const_int64_column column_table::get_int64_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::uint64_type));
  return columns_[column_id].as_int64_column();
}
float_column column_table::get_float_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::float_type));
  return columns_[column_id].as_float_column();
}
const_float_column column_table::get_float_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::float_type));
  return columns_[column_id].as_float_column();
}
double_column column_table::get_double_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::double_type));
  return columns_[column_id].as_double_column();
}
const_double_column column_table::get_double_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::double_type));
  return columns_[column_id].as_double_column();
}
string_column column_table::get_string_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::string_type));
  return columns_[column_id].as_string_column();
}
const_string_column column_table::get_string_column(size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::string_type));
  return columns_[column_id].as_string_column();
}
bit_vector_column column_table::get_bit_vector_column(size_t column_id) {
  assert(columns_[column_id].type().is(column_type::bit_vector_type));
  return columns_[column_id].as_bit_vector_column();
}
const_bit_vector_column column_table::get_bit_vector_column(
    size_t column_id) const {
  assert(columns_[column_id].type().is(column_type::bit_vector_type));
  return columns_[column_id].as_bit_vector_column();
}


}  // namespace table
}  // namespace jubatus
