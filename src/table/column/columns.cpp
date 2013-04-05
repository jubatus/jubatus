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

#include <string>
#include "columns.hpp"
#include <assert.h>
namespace jubatus {
namespace table {
namespace detail {

void abstruct_column::push_back(const uint8_t& value) {
  assert(type_.is(column_type::uint8_type));
  prepare_append(type_.size());
  uint8_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const uint16_t& value) {
  assert(type_.is(column_type::uint16_type));
  prepare_append(type_.size());
  uint16_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const uint32_t& value) {
  assert(type_.is(column_type::uint32_type));
  prepare_append(type_.size());
  uint32_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const uint64_t& value) {
  assert(type_.is(column_type::uint64_type));
  prepare_append(type_.size());
  uint64_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const int8_t& value) {
  assert(type_.is(column_type::int8_type));
  prepare_append(type_.size());
  int8_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const int16_t& value) {
  assert(type_.is(column_type::int16_type));
  prepare_append(type_.size());
  int16_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const int32_t& value) {
  assert(type_.is(column_type::int32_type));
  prepare_append(type_.size());
  int32_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const int64_t& value) {
  assert(type_.is(column_type::int64_type));
  prepare_append(type_.size());
  int64_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const std::string& value) {
  assert(type_.is(column_type::string_type));
  prepare_append(type_.size());
  string_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const float& value) {
  assert(type_.is(column_type::float_type));
  prepare_append(type_.size());
  float_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const double& value) {
  assert(type_.is(column_type::double_type));
  prepare_append(type_.size());
  double_column target(ptr_, size_);
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::push_back(const bit_vector& value) {
  assert(type_.is(column_type::bit_vector_type));
  prepare_append(type_.size());
  bit_vector_column target(ptr_, size_, type_.bit_vector_length());
  target.push_back(value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const uint8_t& value) {
  assert(type_.is(column_type::uint8_type));
  prepare_append(type_.size());
  uint8_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const uint16_t& value) {
  assert(type_.is(column_type::uint16_type));
  prepare_append(type_.size());
  uint16_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const uint32_t& value) {
  assert(type_.is(column_type::uint32_type));
  prepare_append(type_.size());
  uint32_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const uint64_t& value) {
  assert(type_.is(column_type::uint64_type));
  prepare_append(type_.size());
  uint64_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const int8_t& value) {
  assert(type_.is(column_type::int8_type));
  prepare_append(type_.size());
  int8_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const int16_t& value) {
  assert(type_.is(column_type::int64_type));
  prepare_append(type_.size());
  int16_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const int32_t& value) {
  assert(type_.is(column_type::int32_type));
  prepare_append(type_.size());
  int32_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const int64_t& value) {
  assert(type_.is(column_type::int64_type));
  prepare_append(type_.size());
  int64_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const std::string& value) {
  assert(type_.is(column_type::string_type));
  prepare_append(type_.size());
  string_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const float& value) {
  assert(type_.is(column_type::float_type));
  prepare_append(type_.size());
  float_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const double& value) {
  assert(type_.is(column_type::double_type));
  prepare_append(type_.size());
  double_column target(ptr_, size_);
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::insert(uint64_t index, const bit_vector& value) {
  assert(type_.is(column_type::bit_vector_type));
  prepare_append(type_.size());
  bit_vector_column target(ptr_, size_, type_.bit_vector_length());
  target.insert(index, value);
  size_ += type_.size();
}

void abstruct_column::update(uint64_t index, const uint8_t& value) {
  assert(type_.is(column_type::uint8_type));
  uint8_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const uint16_t& value) {
  assert(type_.is(column_type::uint16_type));
  uint16_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const uint32_t& value) {
  assert(type_.is(column_type::uint32_type));
  uint32_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const uint64_t& value) {
  assert(type_.is(column_type::uint64_type));
  uint64_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const int8_t& value) {
  assert(type_.is(column_type::int8_type));
  int8_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const int16_t& value) {
  assert(type_.is(column_type::int16_type));
  int16_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const int32_t& value) {
  assert(type_.is(column_type::int32_type));
  int32_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const int64_t& value) {
  assert(type_.is(column_type::int64_type));
  int64_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const std::string& value) {
  assert(type_.is(column_type::string_type));
  string_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const float& value) {
  assert(type_.is(column_type::float_type));
  float_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const double& value) {
  assert(type_.is(column_type::double_type));
  double_column target(ptr_, size_);
  target.update(index, value);
}

void abstruct_column::update(uint64_t index, const bit_vector& value) {
  assert(type_.is(column_type::bit_vector_type));
  bit_vector_column target(ptr_, size_, type_.bit_vector_length());
  target[index] = value;
}

}  // namespace detail
}  // namespace table
}  // namespace jubatus
