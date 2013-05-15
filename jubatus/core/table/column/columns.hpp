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

#ifndef JUBATUS_CORE_TABLE_COLUMN_COLUMNS_HPP_
#define JUBATUS_CORE_TABLE_COLUMN_COLUMNS_HPP_

#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <msgpack.hpp>
#include <pficommon/lang/demangle.h>
#include <pficommon/data/serialization.h>
#include "../storage_exception.hpp"
#include "bit_vector.hpp"
#include "column_type.hpp"

namespace jubatus {
namespace core {
namespace table {
namespace detail {
class abstract_column;
}

#define EMBED_TYPES                             \
  TYPE(int8)                                    \
  TYPE(int16)  /* NOLINT */                     \
  TYPE(int32)  /* NOLINT */                     \
  TYPE(int64)  /* NOLINT */                     \
  TYPE(uint8)                                   \
  TYPE(uint16)  /* NOLINT */                    \
  TYPE(uint32)  /* NOLINT */                    \
  TYPE(uint64)  /* NOLINT */                    \
  TYPE(float)  /* NOLINT */                     \
  TYPE(double)  /* NOLINT */                    \
  TYPE(string)
#define USE_TYPES_NEED_T                        \
  TYPE(int8)                                    \
  TYPE(int16)  /* NOLINT */                     \
  TYPE(int32)  /* NOLINT */                      \
  TYPE(int64)  /* NOLINT */                      \
  TYPE(uint8)                                   \
  TYPE(uint16)  /* NOLINT */                    \
  TYPE(uint32)  /* NOLINT */                    \
  TYPE(uint64)  // NOLINT
#define USE_TYPES_UNNEED_T                      \
  TYPE(float)  /* NOLINT */                     \
  TYPE(double)  /* NOLINT */                    \
  TYPE(string)


struct bit_vector_column {
  bit_vector_column(char* ptr, uint64_t size, size_t bit_num)
      : ptr_(reinterpret_cast<char*>(ptr)),
        size_(size),
        vector_bit_num_(bit_num) {
  }
  bit_vector_column(const bit_vector_column& orig)
      : ptr_(orig.ptr_),
        size_(orig.size_),
        vector_bit_num_(orig.vector_bit_num_) {
  }
  bit_vector operator[](uint64_t index) const {
    if (size() <= index) {
      throw array_range_exception(
          "index " + pfi::lang::lexical_cast<std::string>(index) +
          " is over length from " +
          pfi::lang::lexical_cast<std::string>(size()));
    }
    return bit_vector(
        &ptr_[bit_vector::memory_size(vector_bit_num_) * index],
        vector_bit_num_);
  }
  bit_vector operator[](uint64_t index) {
    if (size() <= index) {
      throw array_range_exception(
          "index " + pfi::lang::lexical_cast<std::string>(index) +
          " is over length from " +
          pfi::lang::lexical_cast<std::string>(size()));
    }
    return bit_vector(
        &ptr_[bit_vector::memory_size(vector_bit_num_) * index],
        vector_bit_num_);
  }
  void push_back(const bit_vector& orig) {
    bit_vector new_bit_vector(&ptr_[size_], vector_bit_num_);
    new_bit_vector = orig;
  }

  template <typename U>
  void push_back(const U& v) {
    throw type_unmatch_exception(
        "invalid type push_backed: " + pfi::lang::get_typename<U>() +
        " for bit_vector");
  }

  void insert(uint64_t index, const bit_vector& value) {
    const uint64_t memory_size = bit_vector::memory_size(vector_bit_num_);
    char* const target = ptr_ + index*memory_size;
    std::memmove(
        target + bit_vector::memory_size(vector_bit_num_),
        target, size_ - index*memory_size);
    size_ += memory_size;
    bit_vector new_bit_vector(target, vector_bit_num_);
    new_bit_vector = value;
  }

  template <typename U>
  void insert(uint64_t, const U& v) {
    throw type_unmatch_exception(
        "invalid type push_backed: " + pfi::lang::get_typename<U>() +
        " for bit_vector");
  }

  void dump() const {
    std::cout << "[column (bit_vector)"
              << " size:" << size() << " {" << std::endl;
    for (size_t i = 0; i <  size(); ++i) {
      std::cout << "[" << i << "] " << (operator[](i)) << std::endl;
    }
    std::cout << "} ]" << std::endl;
  }

  uint64_t size() const {
    return size_ / bit_vector::memory_size(vector_bit_num_);
  }
  friend std::ostream& operator<<(
      std::ostream& os,
      const bit_vector_column& bv) {
    for (size_t i = 0; i <  bv.size(); ++i) {
      os << bv[i];
    }
    return os;
  }

  void clear() {
    // we dont need to delete
  }

 private:
  char* ptr_;
  uint64_t size_;
  size_t vector_bit_num_;
};

// TODO(beam2d): Stop using private inheritance.
struct const_bit_vector_column : private bit_vector_column {
  const_bit_vector_column(
      const char* ptr,
      const uint64_t size,
      size_t bit_num)
      : bit_vector_column(const_cast<char*>(ptr), size, bit_num) {
  }
  const_bit_vector_column(const bit_vector_column& orig)  // NOLINT implicit!
      : bit_vector_column(orig) {
  }
  using bit_vector_column::operator[];
  using bit_vector_column::dump;
  using bit_vector_column::size;
  friend std::ostream& operator<<(
      std::ostream& os,
      const const_bit_vector_column& bv) {
    for (size_t i = 0; i <  bv.size(); ++i) {
      os << bv[i];
    }
    return os;
  }
};



template<typename T>
class const_typed_column;
template<typename T>
class typed_column {
 private:
  // this type is used only through cast
  // typed_column();
  // typed_column(const typed_column&);
  typed_column& operator=(const typed_column&) const;

 public:
  typed_column(char* ptr, uint64_t size)
      : ptr_(reinterpret_cast<T*>(ptr)), size_(size) {
  }

 private:
  typedef typed_column<T> typed_column_t;
  void push_back(const T& v) {
    T* const target =
        reinterpret_cast<T*>(&(reinterpret_cast<char*>(ptr_)[size_]));
    size_ += sizeof(T);
    new(target) T(v);
  }

  template <typename U>
  void push_back(const U& v) {
    throw type_unmatch_exception(
        "invalid type push_backed: " + pfi::lang::get_typename<U>() +
        " for " + pfi::lang::get_typename<T>());
  }

  void insert(uint64_t index, const T& v) {
    T* const target = ptr_ + index;
    std::memmove(target + 1, target, size_ - index*sizeof(T));
    size_ += sizeof(T);
    new(target) T(v);
  }

  template <typename U>
  void insert(uint64_t index, const U& v) {
    throw type_unmatch_exception(
        "invalid type inserted: " + pfi::lang::get_typename<U>() +
        " for " + pfi::lang::get_typename<T>());
  }

  bool update(uint64_t index, const T& orig) {
    if (size() < index) {
      return false;
    }
    T* const target = ptr_ + index;
    target->~T();
    new(target) T(orig);
    return true;
  }

  template<typename U>
  bool update(uint64_t index, const U& orig) {
    throw type_unmatch_exception(
        "invalid type update(): " + pfi::lang::get_typename<U>() +
        " for " + pfi::lang::get_typename<T>());
  }

 public:
  void dump() const {
    std::cout << "[column (" << pfi::lang::get_typename<T>() << ")"
              << " size:" << size_ << " {";
    for (size_t i = 0; i < size_; ++i) {
      std::cout << operator[](i);
      std::cout << " ";
    }
    std::cout << "} ]";
  }

  T& operator[](uint64_t index) {
    if (size() <= index) {
      throw array_range_exception(
          "index " + pfi::lang::lexical_cast<std::string>(index) +
          " is over length from " +
          pfi::lang::lexical_cast<std::string>(size()));
    }
    return ptr_[index];
  }
  const T& operator[](uint64_t index) const {
    if (size() <= index) {
      throw array_range_exception(
          "index " + pfi::lang::lexical_cast<std::string>(index) +
          " is over length from "
          + pfi::lang::lexical_cast<std::string>(size()));
    }
    return ptr_[index];
  }

  void clear() {
    assert(ptr_ != NULL);
    T* target = ptr_;
    for (uint64_t i = 0; i < size(); ++i) {
      target->~T();
      ++target;
    }
  }

  uint64_t size() const {
    return size_ / sizeof(T);
  }
  friend std::ostream& operator<<(
      std::ostream& os,
      const typed_column_t& column) {
    os << "size: " << *column.size_ << std::endl;
    for (uint64_t i = 0; i < column.size(); ++i) {
      os << i << ":" << column[i] << std::endl;
    }
    return os;
  }

 protected:
  T* ptr_;
  uint64_t size_;
  friend class detail::abstract_column;

 private:
  friend class const_typed_column<T>;
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(size_);
    for (uint64_t i = 0; i < size_; ++i) {
      ar & NAMED_MEMBER(
          pfi::lang::lexical_cast<std::string>(i), this->operator[](i));
    }
  }
};

template<typename T>
class const_typed_column : private typed_column<T> {
  typedef typed_column<T> base_typed_column;

 public:
  typedef const_typed_column<T> typed_column_t;
  const_typed_column(const char* ptr, uint64_t size)
      : base_typed_column(const_cast<char*>(ptr), size) {
  }
  const_typed_column(const typed_column<T>& orig)  // NOLINT implicit!
      : typed_column<T>(orig) {
  }
  using base_typed_column::dump;
  using base_typed_column::operator[];
  using base_typed_column::size;
  friend std::ostream& operator<<(
      std::ostream& os,
      const typed_column_t& column) {
    os << "size: " << column.size_ << std::endl;
    for (uint64_t i = 0; i < column.size(); ++i) {
      os << i << ":" << column[i] << std::endl;
    }
    return os;
  }

 private:
  friend class pfi::data::serialization::access;
  using base_typed_column::serialize;
};

#define SUFFIX(x) x##_t
#define COLUMN_T(ctype)                                             \
  typedef typed_column<SUFFIX(ctype)> ctype##_column;               \
  typedef const_typed_column<SUFFIX(ctype)> const_##ctype##_column;
#define TYPE(x) COLUMN_T(x)
USE_TYPES_NEED_T
#undef TYPE
#undef SUFFIX
#define SUFFIX(x) x
COLUMN_T(float);  // NOLINT
COLUMN_T(double);  // NOLINT
#undef SUFFIX
#undef COLUMN_T
typedef typed_column<std::string> string_column;
typedef const_typed_column<std::string> const_string_column;

namespace detail {
class abstract_column {
 public:
  abstract_column()
      : type_(column_type::invalid_type),
        ptr_(NULL),
        reserved_(0),
        size_(0),
        destroy_duty_(false) {
  }
  explicit abstract_column(const column_type& type)
      : type_(type),
        ptr_(NULL),
        reserved_(0),
        size_(0),
        destroy_duty_(false) {
  }

  ~abstract_column() {
    if (destroy_duty_ && ptr_ != NULL) {
      if (is_string()) {
        string_column sc(ptr_, size_);
        sc.clear();
      }
      delete[] ptr_;
    }
  }
  void push_back(const uint8_t& value);
  void push_back(const uint16_t& value);
  void push_back(const uint32_t& value);
  void push_back(const uint64_t& value);
  void push_back(const int8_t& value);
  void push_back(const int16_t& value);
  void push_back(const int32_t& value);
  void push_back(const int64_t& value);
  void push_back(const std::string& value);
  void push_back(const float& value);
  void push_back(const double& value);
  void push_back(const bit_vector& value);

  void push_back(const msgpack::object& o) {
    prepare_append(type_.size());
#define COLUMN(type)                                \
    if (type_.is(column_type::type##_type)) {       \
      type##_column column(ptr_, size_);            \
        SUFFIX(type) t;                             \
        o.convert(&t);                              \
        column.push_back(t);                        \
    } else  // NOLINT
#define TYPE(x) COLUMN(x)
#define SUFFIX(x) x##_t
    USE_TYPES_NEED_T
#undef SUFFIX
#define SUFFIX(x) x
      USE_TYPES_UNNEED_T
#undef SUFFIX
#undef TYPE
      if (type_.is(column_type::bit_vector_type)) {
        bit_vector_column target(ptr_, size_, type_.bit_vector_length());
        bit_vector bv(type_.bit_vector_length());
        bv.alloc_memory();
        if (o.type != msgpack::type::RAW) {
          throw std::bad_cast();
        }
        std::memcpy(bv.raw_data_unsafe(), o.via.raw.ptr, o.via.raw.size);
        target.push_back(bv);
      } else {
        throw std::bad_cast();
      }
#undef COLUMN
    size_ += type_.size();
  }

  void insert(uint64_t index, const uint8_t& value);
  void insert(uint64_t index, const uint16_t& value);
  void insert(uint64_t index, const uint32_t& value);
  void insert(uint64_t index, const uint64_t& value);
  void insert(uint64_t index, const int8_t& value);
  void insert(uint64_t index, const int16_t& value);
  void insert(uint64_t index, const int32_t& value);
  void insert(uint64_t index, const int64_t& value);
  void insert(uint64_t index, const float& value);
  void insert(uint64_t index, const double& value);
  void insert(uint64_t index, const std::string& value);
  void insert(uint64_t index, const bit_vector& value);

  void update(uint64_t index, const uint8_t& value);
  void update(uint64_t index, const uint16_t& value);
  void update(uint64_t index, const uint32_t& value);
  void update(uint64_t index, const uint64_t& value);
  void update(uint64_t index, const int8_t& value);
  void update(uint64_t index, const int16_t& value);
  void update(uint64_t index, const int32_t& value);
  void update(uint64_t index, const int64_t& value);
  void update(uint64_t index, const std::string& value);
  void update(uint64_t index, const float& value);
  void update(uint64_t index, const double& value);
  void update(uint64_t index, const bit_vector& value);

  void update(uint64_t target, const msgpack::object& o) {
#define COLUMN(type)                                \
    if (type_.is(column_type::type##_type)) {       \
      type##_column column(ptr_, size_);            \
        SUFFIX(type) t;                             \
        o.convert(&t);                              \
        column.update(target, t);                    \
    } else  // NOLINT
#define TYPE(x) COLUMN(x)
#define SUFFIX(x) x##_t
    USE_TYPES_NEED_T
#undef SUFFIX
#define SUFFIX(x) x
      USE_TYPES_UNNEED_T
#undef SUFFIX
#undef TYPE
      if (type_.is(column_type::bit_vector_type)) {
        bit_vector_column column(ptr_, size_, type_.bit_vector_length());
        bit_vector bv(type_.bit_vector_length());
        bv.alloc_memory();
        if (o.type != msgpack::type::RAW) {
          throw std::bad_cast();
        }
        std::memcpy(bv.raw_data_unsafe(), o.via.raw.ptr, o.via.raw.size);
        column[target] = bv;
      } else {
        throw std::bad_cast();
      }
#undef COLUMN
  }

  const column_type& type() const {return type_;}

#define COLUMN_T(ctype)                                                 \
  bool is_##ctype() const {return type_.is(column_type::ctype##_type);} \
  template<typename U>                                                  \
  bool type_##ctype() {                                                 \
    return typeid(U) == typeid(SUFFIX(ctype)) && is_##ctype();          \
  }                                                                     \
  ctype##_column as_##ctype##_column() {                                \
    return ctype##_column(ptr_, size_ SIZE_ARG(d));                     \
  }                                                                     \
  const_##ctype##_column as_##ctype##_column() const {                  \
    return const_##ctype##_column(ptr_, size_ SIZE_ARG(d));             \
  }
# define SIZE_ARG(x)
# define SUFFIX(x) x##_t
  COLUMN_T(int8);
  COLUMN_T(int16);  // NOLINT
  COLUMN_T(int32);  // NOLINT
  COLUMN_T(int64);  // NOLINT
  COLUMN_T(uint8);
  COLUMN_T(uint16);  // NOLINT
  COLUMN_T(uint32);  // NOLINT
  COLUMN_T(uint64);  // NOLINT
# undef SUFFIX
# define SUFFIX(x) x
  COLUMN_T(float);  // NOLINT
  COLUMN_T(double);  // NOLINT
  COLUMN_T(string);
#  undef SIZE_ARG
#  define SIZE_ARG(x) , type_.bit_vector_length()
  COLUMN_T(bit_vector);
#  undef SIZE_ARG
# undef SUFFIX
#undef COLUMN_T

  template<typename T1>
  bool is_type() const {
#define IS_TYPE(ctype) (typeid(T1) == typeid(SUFFIX(ctype)) && is_##ctype()) ||
#define SUFFIX(x) x##_t
    return IS_TYPE(int8) IS_TYPE(int16) IS_TYPE(int32)  // NOLINT
        IS_TYPE(int64) IS_TYPE(uint8) IS_TYPE(uint16)  // NOLINT
        IS_TYPE(uint32) IS_TYPE(uint64)  // NOLINT
#undef SUFFIX
#define SUFFIX(x) x
        IS_TYPE(float) IS_TYPE(double) IS_TYPE(string)  // NOLINT
        IS_TYPE(bit_vector)  // NOLINT
#undef SUFFIX
      0;
  }

  uint64_t size() const {return size_;}

  void clear() {
#define COLUMN(type)                                \
    if (type_.is(column_type::type##_type)) {       \
      type##_column column(ptr_, size_); \
        column.clear();                             \
    } else  // NOLINT
#define TYPE(x) COLUMN(x)
    EMBED_TYPES
      if (type_.is(column_type::bit_vector_type)) {
        bit_vector_column column(ptr_, size_, type_.bit_vector_length());
        column.clear();
      } else {
        throw type_unmatch_exception(
            "clear(): invalid type " + type().type_as_string());
      }
#undef TYPE
#undef COLUMN
  }

  template <typename packer>
  void pack_with_index(const uint64_t index, packer& pk) const {
#define PACK_COLUMN(type)                                   \
    if (type_.is(column_type::type##_type)) {               \
      const_##type##_column column(ptr_, size_);            \
      pk.pack(column[index]);                               \
    } else  // NOLINT
#define TYPE(x) PACK_COLUMN(x)
    EMBED_TYPES
#undef TYPE
      if (type_.is(column_type::bit_vector_type)) {
        const_bit_vector_column column(
            ptr_, size_, type_.bit_vector_length());
        const bit_vector bv(column[index]);
        pk.pack_raw(bv.used_bytes());
        pk.pack_raw_body(
            reinterpret_cast<const char*>(bv.raw_data_unsafe()),
            bv.used_bytes());
      } else {
        throw type_unmatch_exception(
            "dump(int): invalid type " + type().type_as_string());
      }
#undef PACK_COLUMN
  }

  void dump() const {
    std::cout << "[ ";
    type_.dump();
    std::cout << " size:" << size_ << " ]";
  }
  void dump(std::ostream& os, uint64_t index) const {
#define SAVE_COLUMN(type)                                   \
    if (type_.is(column_type::type##_type)) {               \
      const_##type##_column column(ptr_, size_);            \
      os << column[index];                                  \
    } else  // NOLINT
#define TYPE(x) SAVE_COLUMN(x)
    EMBED_TYPES
      if (type_.is(column_type::bit_vector_type)) {
        const_bit_vector_column column(
            ptr_, size_, type_.bit_vector_length());
        os << column[index];
      } else {
        throw type_unmatch_exception(
            "dump(int): invalid type " + type().type_as_string());
      }
#undef TYPE
#undef SAVE_COLUMN
  }

  void swap(abstract_column& rhs) {
    using std::swap;
    swap(type_, rhs.type_);
    swap(ptr_, rhs.ptr_);
    swap(reserved_, rhs.reserved_);
    swap(size_, rhs.size_);
    swap(destroy_duty_, rhs.destroy_duty_);
  }

  void reserve(size_t expect_size) {
    if (ptr_ == NULL) {
      assert(destroy_duty_ == false);
      reserved_ = std::max(expect_size, size_t(64U));
      size_ = 0;
      destroy_duty_ = true;
      ptr_ = reinterpret_cast<char*>(new align_dummy[reserved_ >> 4]);
      return;
    }
    if (reserved_ <= expect_size) {
      while (reserved_ <= expect_size) {
        reserved_ *= 2;
      }
      abstract_column new_column(type_, reserved_);
      std::memcpy(new_column.ptr_, ptr_, size_);
      new_column.size_ = size_;
      new_column.destroy_duty_ = true;
      destroy_duty_ = false;
      swap(new_column);  // new_column.ptr_ is to delete
    }
  }

 private:
  typedef std::string string;
  struct align_dummy {
    double d[2];  // strictry alignied data
  } __attribute__((aligned(16)));

  // for extend only
  abstract_column(const column_type& type, size_t memory_size)
      : type_(type),
        reserved_(memory_size),
        size_(0),
        destroy_duty_(false) {
    ptr_ = reinterpret_cast<char*>(new align_dummy[reserved_ >> 4]);
  }

  void prepare_append(int newsize) {
    reserve(size_ + newsize);
  }

  friend std::ostream& operator<<(
      std::ostream& os,
      const abstract_column& obj) {
    os << obj.type_;
#define SAVE_COLUMN(type)                                               \
    if (obj.type_.is(column_type::type##_type)) {                       \
      const_##type##_column column(obj.ptr_, obj.size_);                \
        os << "column_type: " << obj.type_.type_as_string() << ":";     \
        os << column;                                                   \
    } else  // NOLINT
#define TYPE(x) SAVE_COLUMN(x)
    EMBED_TYPES
      if (obj.type_.is(column_type::bit_vector_type)) {
        const_bit_vector_column column(
            obj.ptr_, obj.size_, obj.type().bit_vector_length());
        os << "column_type: bit_vector :";
        os << column;
      } else {
        throw type_unmatch_exception(
            "operator<<(): invalid type " + obj.type().type_as_string());
      }
#undef TYPE
#undef SAVE_COLUMN
    return os;
  }

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(type_);
    ar & MEMBER(size_);
    // std::cout << "serializing:" << size_ << std::endl;
    std::vector<uint32_t> vec;
    if (ar.is_read) {
      if (ptr_) {
        delete[] ptr_;
      }
      ptr_ = new char[size_];
      reserved_ = size_;
    }

    for (uint64_t i = 0; i < size_; ++i) {
      vec.push_back(static_cast<uint32_t>(ptr_[i]));
    }
    ar & NAMED_MEMBER("ptr_", vec);
    if (ar.is_read) {
      for (uint64_t i = 0; i < size_; ++i) {
        ptr_[i]=static_cast<char>(vec[i]);
      }
    }
  }

  column_type type_;
  char* ptr_;
  uint64_t reserved_;
  uint64_t size_;
  bool destroy_duty_;
};

}  // namespace detail
}  // namespace table
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_TABLE_COLUMN_COLUMNS_HPP_
