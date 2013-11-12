// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012,2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "column_table.hpp"
#include "../../common/assert.hpp"

namespace jutil = jubatus::util;

namespace jubatus {
namespace core {
namespace table {

void column_table::init(const std::vector<column_type>& schema) {
  jubatus::util::concurrent::scoped_lock lk(wlock(table_lock_));
  /* defining tuple */
  if (!columns_.empty()) {
    throw storage_exception(
        "Storage's schemas cannot be over written. Clear schemas before "
        "init()");
  }
  for (std::vector<column_type>::const_iterator it = schema.begin();
       it != schema.end();
       ++it) {
    columns_.push_back(detail::abstract_column(*it));
  }
}

void column_table::clear() {
  jutil::concurrent::scoped_lock lk(wlock(table_lock_));
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
  jutil::concurrent::scoped_lock lk(jutil::concurrent::rlock(table_lock_));
  index_table::const_iterator it = index_.find(prefix);
  if (it == index_.end()) {
    return std::make_pair(false, 0LLU);
  } else {
    return std::make_pair(true, it->second);
  }
}

uint8_column& column_table::get_uint8_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint8_type));
  return *static_cast<uint8_column*>(columns_[column_id].get());
}
const_uint8_column& column_table::get_uint8_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint8_type));
  return *static_cast<const uint8_column*>(columns_[column_id].get());
}
uint16_column& column_table::get_uint16_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint16_type));
  return *static_cast<uint16_column*>(columns_[column_id].get());
}
const_uint16_column& column_table::get_uint16_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint16_type));
  return *static_cast<const uint16_column*>(columns_[column_id].get());
}
uint32_column& column_table::get_uint32_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint32_type));
  return *static_cast<uint32_column*>(columns_[column_id].get());
}
const_uint32_column& column_table::get_uint32_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint32_type));
  return *static_cast<const_uint32_column*>(columns_[column_id].get());
}
uint64_column& column_table::get_uint64_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint64_type));
  return *static_cast<uint64_column*>(columns_[column_id].get());
}
const_uint64_column& column_table::get_uint64_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::uint64_type));
  return *static_cast<const_uint64_column*>(columns_[column_id].get());
}
int8_column& column_table::get_int8_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int8_type));
  return *static_cast<int8_column*>(columns_[column_id].get());
}
const_int8_column& column_table::get_int8_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int8_type));
  return *static_cast<const_int8_column*>(columns_[column_id].get());
}
int16_column& column_table::get_int16_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int16_type));
  return *static_cast<int16_column*>(columns_[column_id].get());
}
const_int16_column& column_table::get_int16_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int16_type));
  return *static_cast<const_int16_column*>(columns_[column_id].get());
}
int32_column& column_table::get_int32_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int32_type));
  return *static_cast<int32_column*>(columns_[column_id].get());
}
const_int32_column& column_table::get_int32_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int32_type));
  return *static_cast<const_int32_column*>(columns_[column_id].get());
}
int64_column& column_table::get_int64_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int64_type));
  return *static_cast<int64_column*>(columns_[column_id].get());
}
const_int64_column& column_table::get_int64_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::int64_type));
  return *static_cast<const_int64_column*>(columns_[column_id].get());
}
float_column& column_table::get_float_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::float_type));
  return *static_cast<float_column*>(columns_[column_id].get());
}
const_float_column& column_table::get_float_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::float_type));
  return *static_cast<const_float_column*>(columns_[column_id].get());
}
double_column& column_table::get_double_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::double_type));
  return *static_cast<double_column*>(columns_[column_id].get());
}
const_double_column& column_table::get_double_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::double_type));
  return *static_cast<const_double_column*>(columns_[column_id].get());
}
string_column& column_table::get_string_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::string_type));
  return *static_cast<string_column*>(columns_[column_id].get());
}
const_string_column& column_table::get_string_column(size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::string_type));
  return *static_cast<const_string_column*>(columns_[column_id].get());
}
bit_vector_column& column_table::get_bit_vector_column(size_t column_id) {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::bit_vector_type));
  return *static_cast<bit_vector_column*>(columns_[column_id].get());
}
const_bit_vector_column& column_table::get_bit_vector_column(
    size_t column_id) const {
  JUBATUS_ASSERT(columns_[column_id].type().is(column_type::bit_vector_type));
  return *static_cast<const_bit_vector_column*>(columns_[column_id].get());
}


}  // namespace table
}  // namespace core
}  // namespace jubatus
