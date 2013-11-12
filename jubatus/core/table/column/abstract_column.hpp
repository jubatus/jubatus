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
#include "jubatus/util/lang/demangle.h"
#include "jubatus/util/lang/noncopyable.h"
#include "../../common/assert.hpp"
#include "../storage_exception.hpp"
#include "bit_vector.hpp"
#include "column_type.hpp"

namespace jubatus {
namespace core {
namespace table {

namespace detail {

class abstract_column_base : jubatus::util::lang::noncopyable {
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
        "actual: " + jubatus::util::lang::get_typename<tp>());    \
    }                                                   \
    virtual bool insert(uint64_t target, const tp&) {   \
      throw type_unmatch_exception(                     \
        "column: invalid type in insert(): "            \
        "expected: " + my_type_.type_as_string() + ", " \
        "actual: " + jubatus::util::lang::get_typename<tp>());    \
    }                                                   \
    virtual bool update(uint64_t target, const tp&) {   \
      throw type_unmatch_exception(                     \
        "column: invalid type in update(): "            \
        "expected: " + my_type_.type_as_string() + ", " \
        "actual: " + jubatus::util::lang::get_typename<tp>());    \
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
        "invalid index [" +
        jubatus::util::lang::lexical_cast<std::string>(index) +
        "] for [" +
        jubatus::util::lang::lexical_cast<std::string>(array_.size()));
    }
    return array_[index];
  }

  T& operator[](uint64_t index) {
    if (size() <= index) {
      throw length_unmatch_exception(
        "invalid index [" +
        jubatus::util::lang::lexical_cast<std::string>(index) +
        "] for [" +
        jubatus::util::lang::lexical_cast<std::string>(array_.size()));
    }
    return array_[index];
  }

  void pack_with_index(
      const uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
    pk.pack((*this)[index]);
  }

  void dump() const {
    std::cout << "[column (" << type().type_as_string() << ")"
        << " size:" << size() << " {" << std::endl;
    for (size_t i = 0; i <  size(); ++i) {
      dump(std::cout, i);
    }
    std::cout << "} ]" << std::endl;
  }
  void dump(std::ostream& os, uint64_t target) const {
    os << "[" << target << "] " << (*this)[target] << std::endl;
  }

  template<class Buffer>
  void pack_array(msgpack::packer<Buffer>& packer) const {
    packer.pack(array_);
  }
  void unpack_array(msgpack::object o) {
    o.convert(&array_);
  }

 private:
  friend class jubatus::util::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    column_type my_type = type();
    ar & NAMED_MEMBER("my_type_", my_type);
    if (my_type != type()) {
      throw type_unmatch_exception(
        "column: invalid type in serialize(): "
        "expected: " + type().type_as_string() + ", "
        "actual: " + my_type.type_as_string());
    }
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
    check_bit_vector_(value);
    array_.resize(array_.size() + blocks_per_value_());
    update_at_(size() - 1, value.raw_data_unsafe());
  }
  void push_back(const msgpack::object& obj) {
    bit_vector value(type().bit_vector_length());
    obj.convert(&value);
    typed_column::push_back(value);
  }

  bool insert(uint64_t target, const bit_vector& value) {
    check_bit_vector_(value);

    if (size() < target) {
      return false;
    }
    array_.insert(
        array_.begin() + target * blocks_per_value_(),
        blocks_per_value_(), 0);
    update_at_(target, value.raw_data_unsafe());
    return true;
  }
  bool insert(uint64_t target, const msgpack::object& obj) {
    bit_vector value(type().bit_vector_length());
    obj.convert(&value);
    return typed_column::insert(target, value);
  }

  bool update(uint64_t index, const bit_vector& value) {
    check_bit_vector_(value);

    if (size() < index) {
      return false;
    }
    update_at_(index, value.raw_data_unsafe());
    return true;
  }
  bool update(uint64_t index, const msgpack::object& obj) {
    bit_vector value(type().bit_vector_length());
    obj.convert(&value);
    return typed_column::update(index, value);
  }

  uint64_t size() const {
    JUBATUS_ASSERT_EQ(array_.size() % blocks_per_value_(), 0u, "");
    return array_.size() / blocks_per_value_();
  }
  bit_vector operator[](uint64_t index) {
    return bit_vector(get_data_at_(index), type().bit_vector_length());
  }
  bit_vector operator[](uint64_t index) const {
    return bit_vector(get_data_at_(index), type().bit_vector_length());
  }
  bool remove(uint64_t target) {
    if (target >= size()) {
      return false;
    }
    if (target < size() - 1) {
      const void* back = get_data_at_(size() - 1);
      memcpy(get_data_at_(target), back, bytes_per_value_());
    }
    JUBATUS_ASSERT_GE(array_.size(), blocks_per_value_(), "");
    array_.resize(array_.size() - blocks_per_value_());
    return true;
  }
  void clear() {
    array_.clear();
  }
  void pack_with_index(
      const uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
    pk.pack((*this)[index]);
  }
  void dump() const {
    std::cout << "[column (bit_vector)"
        << " size:" << size() << " {" << std::endl;
    for (size_t i = 0; i <  size(); ++i) {
      dump(std::cout, i);
    }
    std::cout << "} ]" << std::endl;
  }
  void dump(std::ostream& os, uint64_t target) const {
    os << "[" << target << "] " << (*this)[target] << std::endl;
  }

  template<class Buffer>
  void pack_array(msgpack::packer<Buffer>& packer) const {
    packer.pack(array_);
  }
  void unpack_array(msgpack::object o) {
    o.convert(&array_);
  }

 private:
  friend class jubatus::util::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    column_type my_type = type();
    ar & NAMED_MEMBER("my_type_", my_type);
    ar & MEMBER(array_);
  }
  std::vector<uint64_t> array_;

  size_t bytes_per_value_() const {
    return bit_vector::memory_size(type().bit_vector_length());
  }
  size_t blocks_per_value_() const {
    return (bytes_per_value_() + sizeof(uint64_t) - 1) / sizeof(uint64_t);
  }

  void* get_data_at_(size_t index) {
    JUBATUS_ASSERT_LT(index, size(), "");
    return &array_[blocks_per_value_() * index];
  }
  const void* get_data_at_(size_t index) const {
    JUBATUS_ASSERT_LT(index, size(), "");
    return &array_[blocks_per_value_() * index];
  }

  void update_at_(size_t index, const void* raw_data) {
    if (raw_data) {
      memcpy(get_data_at_(size() - 1), raw_data, bytes_per_value_());
    } else {
      memset(get_data_at_(size() - 1), 0, bytes_per_value_());
    }
  }

  void check_bit_vector_(const bit_vector& tested) const {
    const size_t bit_num_expected = type().bit_vector_length();
    if (tested.bit_num() > bit_num_expected) {
      throw length_unmatch_exception(
        "invalid length of bit_vector (" +
        jubatus::util::lang::lexical_cast<std::string>(tested.bit_num()) + ", "
        "expected " +
        jubatus::util::lang::lexical_cast<std::string>(bit_num_expected) + ")");
    }
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
  abstract_column() {
  }

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

  void swap(abstract_column& x) {
    base_.swap(x.base_);
  }
  friend void swap(abstract_column& l, abstract_column& r) {
    l.swap(r);
  }

  template<class Buffer>
  void msgpack_pack(msgpack::packer<Buffer>& packer) const {
    column_type type = base_->type();

    packer.pack_array(2);
    packer.pack(type);

    if (type.is(column_type::uint8_type)) {
      static_cast<const uint8_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::uint16_type)) {
      static_cast<const uint16_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::uint32_type)) {
      static_cast<const uint32_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::uint64_type)) {
      static_cast<const uint64_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::int8_type)) {
      static_cast<const int8_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::int16_type)) {
      static_cast<const int16_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::int32_type)) {
      static_cast<const int32_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::int64_type)) {
      static_cast<const int64_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::float_type)) {
      static_cast<const float_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::double_type)) {
      static_cast<const double_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::string_type)) {
      static_cast<const string_column&>(*base_).pack_array(packer);
    } else if (type.is(column_type::bit_vector_type)) {
      static_cast<const bit_vector_column&>(*base_).pack_array(packer);
    } else {
      JUBATUS_ASSERT_UNREACHABLE();
    }
  }
  void msgpack_unpack(msgpack::object o) {
    if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
      throw msgpack::type_error();
    }
    msgpack::object* objs = o.via.array.ptr;

    column_type type;
    objs[0].convert(&type);

    abstract_column tmp;
    if (!base_) {
      abstract_column(type).swap(tmp);  // NOLINT
    } else if (base_->type() == type) {
      this->swap(tmp);
    } else {
      throw type_unmatch_exception(
        "column: invalid type in serialize(): "
        "expected: " +
        jubatus::util::lang::lexical_cast<std::string>(base_->type()) +
        ", actual: " +
        jubatus::util::lang::lexical_cast<std::string>(type));
    }

    if (type.is(column_type::uint8_type)) {
      static_cast<uint8_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::uint16_type)) {
      static_cast<uint16_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::uint32_type)) {
      static_cast<uint32_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::uint64_type)) {
      static_cast<uint64_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::int8_type)) {
      static_cast<int8_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::int16_type)) {
      static_cast<int16_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::int32_type)) {
      static_cast<int32_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::int64_type)) {
      static_cast<int64_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::float_type)) {
      static_cast<float_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::double_type)) {
      static_cast<double_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::string_type)) {
      static_cast<string_column&>(*tmp.base_).unpack_array(objs[1]);
    } else if (type.is(column_type::bit_vector_type)) {
      static_cast<bit_vector_column&>(*tmp.base_).unpack_array(objs[1]);
    } else {
      JUBATUS_ASSERT_UNREACHABLE();
    }

    this->swap(tmp);
  }

 private:
  friend class jubatus::util::data::serialization::access;

  template <class Ar>
  void serialize(Ar& ar) {
    column_type type;

    if (!base_) {
      JUBATUS_ASSERT(ar.is_read);
      ar & NAMED_MEMBER("my_type_", type);
      abstract_column(type).swap(*this);  // NOLINT
    } else {
      type = base_->type();
      ar & NAMED_MEMBER("my_type_", type);
      if (type != base_->type()) {
        throw type_unmatch_exception(
          "column: invalid type in serialize(): "
          "expected: " +
          jubatus::util::lang::lexical_cast<std::string>(base_->type()) +
          ", actual: " +
          jubatus::util::lang::lexical_cast<std::string>(type));
      }
    }

    if (type.is(column_type::uint8_type)) {
      ar & static_cast<uint8_column&>(*base_);
    } else if (type.is(column_type::uint16_type)) {
      ar & static_cast<uint16_column&>(*base_);
    } else if (type.is(column_type::uint32_type)) {
      ar & static_cast<uint32_column&>(*base_);
    } else if (type.is(column_type::uint64_type)) {
      ar & static_cast<uint64_column&>(*base_);
    } else if (type.is(column_type::int8_type)) {
      ar & static_cast<int8_column&>(*base_);
    } else if (type.is(column_type::int16_type)) {
      ar & static_cast<int16_column&>(*base_);
    } else if (type.is(column_type::int32_type)) {
      ar & static_cast<int32_column&>(*base_);
    } else if (type.is(column_type::int64_type)) {
      ar & static_cast<int64_column&>(*base_);
    } else if (type.is(column_type::float_type)) {
      ar & static_cast<float_column&>(*base_);
    } else if (type.is(column_type::double_type)) {
      ar & static_cast<double_column&>(*base_);
    } else if (type.is(column_type::string_type)) {
      ar & static_cast<string_column&>(*base_);
    } else if (type.is(column_type::bit_vector_type)) {
      ar & static_cast<bit_vector_column&>(*base_);
    } else {
      JUBATUS_ASSERT_UNREACHABLE();
    }
  }

  jubatus::util::lang::shared_ptr<abstract_column_base> base_;
};

}  // namespace detail

}  // namespace table
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_TABLE_COLUMN_ABSTRACT_COLUMN_HPP_
