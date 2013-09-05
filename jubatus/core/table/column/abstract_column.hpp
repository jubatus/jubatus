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


#ifndef JUBATUS_CORE_TABLE_COLUMN_ABSTRACT_COLUMN_HPP_
#define JUBATUS_CORE_TABLE_COLUMN_ABSTRACT_COLUMN_HPP_

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <msgpack.hpp>
#include <pficommon/lang/demangle.h>
#include <pficommon/lang/noncopyable.h>
#include "../../common/assert.hpp"
#include "../storage_exception.hpp"
#include "bit_vector.hpp"
#include "column_type.hpp"

namespace jubatus {
namespace core {
namespace table {

namespace detail {

class abstract_column_base : pfi::lang::noncopyable {
 public:
  explicit abstract_column_base(const column_type& type)
      : my_type_(type) {
  }
  virtual ~abstract_column_base() {
  }

  column_type type() const {
    return my_type_;
  }

  #define JUBATUS_GEN_FUNCTIONS_(tp)                    \
    virtual void push_back(const tp& value) {           \
      throw type_unmatch_exception(                     \
        "column: invalid type in push_back(): "         \
        "expected: " + my_type_.type_as_string() + ", " \
        "actual: " + pfi::lang::get_typename<tp>());    \
    }                                                   \
    virtual bool insert(uint64_t target, const tp&) {   \
      throw type_unmatch_exception(                     \
        "column: invalid type in insert(): "            \
        "expected: " + my_type_.type_as_string() + ", " \
        "actual: " + pfi::lang::get_typename<tp>());    \
    }                                                   \
    virtual bool update(uint64_t target, const tp&) {   \
      throw type_unmatch_exception(                     \
        "column: invalid type in update(): "            \
        "expected: " + my_type_.type_as_string() + ", " \
        "actual: " + pfi::lang::get_typename<tp>());    \
    }

    JUBATUS_GEN_FUNCTIONS_(uint8_t);          // NOLINT
    JUBATUS_GEN_FUNCTIONS_(uint16_t);         // NOLINT
    JUBATUS_GEN_FUNCTIONS_(uint32_t);         // NOLINT
    JUBATUS_GEN_FUNCTIONS_(uint64_t);         // NOLINT
    JUBATUS_GEN_FUNCTIONS_(int8_t);           // NOLINT
    JUBATUS_GEN_FUNCTIONS_(int16_t);          // NOLINT
    JUBATUS_GEN_FUNCTIONS_(int32_t);          // NOLINT
    JUBATUS_GEN_FUNCTIONS_(int64_t);          // NOLINT
    JUBATUS_GEN_FUNCTIONS_(float);            // NOLINT
    JUBATUS_GEN_FUNCTIONS_(double);           // NOLINT
    JUBATUS_GEN_FUNCTIONS_(std::string);      // NOLINT
    JUBATUS_GEN_FUNCTIONS_(bit_vector);       // NOLINT
    JUBATUS_GEN_FUNCTIONS_(msgpack::object);  // NOLINT
  #undef JUBATUS_GEN_FUNCTIONS_

  virtual bool remove(uint64_t target) = 0;
  virtual void clear() = 0;
  virtual void pack_with_index(
      const uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
  }
  virtual void dump() const = 0;
  virtual void dump(std::ostream& os, uint64_t target) const = 0;

 private:
  column_type my_type_;
};

}  // namespace detail

template <typename T>
class typed_column : public detail::abstract_column_base {
 public:
  explicit typed_column(const column_type& type)
      : detail::abstract_column_base(type) {
  }

  using detail::abstract_column_base::push_back;
  using detail::abstract_column_base::insert;
  using detail::abstract_column_base::update;

  void push_back(const T& value) {
    array_.push_back(value);
  }
  void push_back(const msgpack::object& obj) {
    typed_column::push_back(obj.as<T>());
  }

  bool insert(uint64_t target, const T& value) {
    if (size() < target) {
      return false;
    }
    array_.insert(array_.begin() + target, value);
    return true;
  }
  bool insert(uint64_t target, const msgpack::object& obj) {
    return typed_column::insert(target, obj.as<T>());
  }

  bool update(uint64_t index, const T& value) {
    if (size() <= index) {
      return false;
    }
    array_[index] = value;
    return true;
  }
  bool update(uint64_t target, const msgpack::object& obj) {
    return typed_column::update(target, obj.as<T>());
  }

  bool remove(uint64_t target) {
    if (size() <= target) {
      return false;
    }
    using std::swap;
    swap(array_[target], array_.back());
    array_.pop_back();
    return true;
  }
  void clear() {
    array_.clear();
  }

  uint64_t size() const {
    return array_.size();
  }

  const T& operator[](uint64_t index) const {
    if (size() <= index) {
      throw length_unmatch_exception(
        "invalid index [" + pfi::lang::lexical_cast<std::string>(index) +
        "] for [" +
        pfi::lang::lexical_cast<std::string>(array_.size()));
    }
    return array_[index];
  }

  T& operator[](uint64_t index) {
    if (size() <= index) {
      throw length_unmatch_exception(
        "invalid index [" + pfi::lang::lexical_cast<std::string>(index) +
        "] for [" +
        pfi::lang::lexical_cast<std::string>(array_.size()));
    }
    return array_[index];
  }

  void dump() const {}
  void dump(std::ostream& os, uint64_t target) const {
  }

 private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(my_type_);
    ar & MEMBER(array_);
  }
  std::vector<T> array_;
};

template <>
class typed_column<bit_vector> : public detail::abstract_column_base {
 public:
  explicit typed_column(const column_type& type)
      : detail::abstract_column_base(type) {
  }

  using detail::abstract_column_base::push_back;
  using detail::abstract_column_base::insert;
  using detail::abstract_column_base::update;

  void push_back(const bit_vector& value) {
    const size_t bit_length = type().bit_vector_length();
    if (value.bit_num() > bit_length) {
      throw length_unmatch_exception(
        "invalid length of bit_vector (" +
        pfi::lang::lexical_cast<std::string>(value.bit_num()) + ", "
        "expected " +
        pfi::lang::lexical_cast<std::string>(bit_length) + ")");
    }
    for (size_t i = 0; i < blocks_per_value_(); ++i) {
      array_.push_back(uint64_t());
    }
    memcpy(
        &array_[array_.size() - blocks_per_value_()],
        value.raw_data_unsafe(),
        value.used_bytes());
  }
  /*
  void push_back(const msgpack::object& obj) {
    array_.push_back(uint64_t());
    obj.convert(&array_[array_.size()]);
  }
  */
  /*
  bool update(uint64_t index, const msgpack::object& obj) {
    if (size() < index) {
      return false;
    }
    obj.convert(&array_[index]);
    return true;
  }
  */

  uint64_t size() const {
    return array_.size() / blocks_per_value_();
  }
  bit_vector operator[](uint64_t index) {
    return bit_vector(
      &array_[blocks_per_value_() * index],
      type().bit_vector_length());
  }
  bit_vector operator[](uint64_t index) const {
    return bit_vector(
      &array_[blocks_per_value_() * index],
      type().bit_vector_length());
  }
  bool remove(uint64_t target) {
    if (target >= size()) {
      return false;
    }
    std::vector<uint64_t>::iterator iter =
        array_.begin() + target * blocks_per_value_();
    array_.erase(iter, iter + blocks_per_value_());
    return true;
  }
  void clear() {
    array_.clear();
  }
  void dump() const {}
  void dump(std::ostream& os, uint64_t target) const {
  }

 private:
  std::vector<uint64_t> array_;
  size_t blocks_per_value_() const {
    size_t bytes_per_value =
        bit_vector::memory_size(type().bit_vector_length());
    return (bytes_per_value + sizeof(uint64_t) - 1) / sizeof(uint64_t);
  }
};

typedef typed_column<uint8_t> uint8_column;
typedef typed_column<uint16_t> uint16_column;
typedef typed_column<uint32_t> uint32_column;
typedef typed_column<uint64_t> uint64_column;
typedef typed_column<int8_t>  int8_column;
typedef typed_column<int16_t> int16_column;
typedef typed_column<int32_t> int32_column;
typedef typed_column<int64_t> int64_column;
typedef typed_column<float>  float_column;
typedef typed_column<double> double_column;
typedef typed_column<std::string> string_column;
typedef typed_column<bit_vector> bit_vector_column;

typedef const typed_column<uint8_t> const_uint8_column;
typedef const typed_column<uint16_t> const_uint16_column;
typedef const typed_column<uint32_t> const_uint32_column;
typedef const typed_column<uint64_t> const_uint64_column;
typedef const typed_column<int8_t>  const_int8_column;
typedef const typed_column<int16_t> const_int16_column;
typedef const typed_column<int32_t> const_int32_column;
typedef const typed_column<int64_t> const_int64_column;
typedef const typed_column<float>  const_float_column;
typedef const typed_column<double> const_double_column;
typedef const typed_column<std::string> const_string_column;
typedef const bit_vector_column const_bit_vector_column;

namespace detail {

class abstract_column {
 public:
  explicit abstract_column(const column_type& type) {
    if (type.is(column_type::uint8_type)) {
      base_.reset(new uint8_column(type));
    } else if (type.is(column_type::uint16_type)) {
      base_.reset(new uint16_column(type));
    } else if (type.is(column_type::uint32_type)) {
      base_.reset(new uint32_column(type));
    } else if (type.is(column_type::uint64_type)) {
      base_.reset(new uint64_column(type));
    } else if (type.is(column_type::int8_type)) {
      base_.reset(new int8_column(type));
    } else if (type.is(column_type::int16_type)) {
      base_.reset(new int16_column(type));
    } else if (type.is(column_type::int32_type)) {
      base_.reset(new int32_column(type));
    } else if (type.is(column_type::int64_type)) {
      base_.reset(new int64_column(type));
    } else if (type.is(column_type::float_type)) {
      base_.reset(new float_column(type));
    } else if (type.is(column_type::double_type)) {
      base_.reset(new double_column(type));
    } else if (type.is(column_type::string_type)) {
      base_.reset(new string_column(type));
    } else if (type.is(column_type::bit_vector_type)) {
      base_.reset(new bit_vector_column(type));
    } else {
      JUBATUS_ASSERT_UNREACHABLE();
    }
  }

  column_type type() const {
    JUBATUS_ASSERT(base_ != NULL);
    return base_->type();
  }

  template <typename T>
  void push_back(const T& value) {
    JUBATUS_ASSERT(base_ != NULL);
    base_->push_back(value);
  }

  template <typename T>
  bool insert(uint64_t index, const T& value) {
    JUBATUS_ASSERT(base_ != NULL);
    return base_->insert(index, value);
  }
  template <typename T>
  bool update(uint64_t index, const T& value) {
    JUBATUS_ASSERT(base_ != NULL);
    return base_->update(index, value);
  }
  bool remove(uint64_t index) {
    JUBATUS_ASSERT(base_ != NULL);
    return base_->remove(index);
  };
  void clear() {
    JUBATUS_ASSERT(base_ != NULL);
    base_->clear();
  }
  void pack_with_index(
      uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
    JUBATUS_ASSERT(base_ != NULL);
    base_->pack_with_index(index, pk);
  }
  abstract_column_base* get() {
    return base_.get();
  }
  const abstract_column_base* get() const {
    return base_.get();
  }

  void dump() const {
    JUBATUS_ASSERT(base_ != NULL);
    base_->dump();
  }
  void dump(std::ostream& os, uint64_t target) const {
    JUBATUS_ASSERT(base_ != NULL);
    base_->dump(os, target);
  }

 private:
  pfi::lang::shared_ptr<abstract_column_base> base_;
};

}  // namespace detail

}  // namespace table
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_TABLE_COLUMN_ABSTRACT_COLUMN_HPP_
