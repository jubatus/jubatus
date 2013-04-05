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

#ifndef JUBATUS_TABLE_COLUMN_COLUMN_TYPE_HPP_
#define JUBATUS_TABLE_COLUMN_COLUMN_TYPE_HPP_

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <pficommon/data/serialization.h>
#include "../storage_exception.hpp"
#include "bit_vector.hpp"

namespace jubatus {
namespace table {

class column_type {
 public:
  enum type_name {
    int8_type,
    int16_type,
    int32_type,
    int64_type,
    uint8_type,
    uint16_type,
    uint32_type,
    uint64_type,
    float_type,
    double_type,
    bit_vector_type,
    string_type,
    array_type,  // not implemented yet
    invalid_type
  };
  column_type():type_(invalid_type), bit_vector_length_(0) {}
  explicit column_type(type_name name)
    :type_(name), bit_vector_length_(0) {
    if (name == bit_vector_type) {
      throw argument_unmatch_exception(
          "bit_vector_type expects bit_vector's length as second argument");
    }
  }
  column_type(type_name name, int length)
    :type_(name), bit_vector_length_(length) {
    if (name != bit_vector_type) {
      throw argument_unmatch_exception(
        "unneeded second parameter, it is used only for bit_vector_type");
    }
  }

  bool is(const type_name& type) const {
    return type_ == type;
  }

  std::string type_as_string() const {
    if (type_ == int8_type) {return "int8";}
#define ELSE_IF_TYPE(ctype)                     \
    else if (type_ == ctype##_type) {           \
      return #ctype;                            \
    }
    ELSE_IF_TYPE(int16)  ELSE_IF_TYPE(int32) ELSE_IF_TYPE(int32)
      ELSE_IF_TYPE(int64) ELSE_IF_TYPE(uint8) ELSE_IF_TYPE(uint16)
      ELSE_IF_TYPE(uint32) ELSE_IF_TYPE(uint32) ELSE_IF_TYPE(uint64)
      ELSE_IF_TYPE(float) ELSE_IF_TYPE(double) ELSE_IF_TYPE(bit_vector)
      ELSE_IF_TYPE(string);
#undef ELSE_IF_TYPE
    throw type_unmatch_exception("in type_as_string(): unknown type");
  }

  size_t size() const {
    switch (type_) {
    case int8_type:
    case uint8_type:
      return 1;
    case int16_type:
    case uint16_type:
      return 2;
    case int32_type:
    case uint32_type:
    case float_type:
      return 4;
    case int64_type:
    case uint64_type:
    case double_type:
    case string_type:
      return 8;
    case bit_vector_type:
      return bit_vector::memory_size(bit_vector_length_);
    case array_type:
    case invalid_type:
    default:
      return 0;
    }
  }


  size_t bit_vector_length() const {
    if (type_ != bit_vector_type) {
      throw type_unmatch_exception("bit_vector length expected but type was" +
                                       type_as_string());
    }
    return bit_vector_length_;
  }
  void dump() const {
    std::cout << "{ " << type_as_string();
    if (type_ == bit_vector_type) {
      std::cout << ": " << bit_vector_length_;
    }
    std::cout << " }";
  }
  column_type& operator=(const column_type& orig) {
    type_ = orig.type_;
    bit_vector_length_ = orig.bit_vector_length_;
    return *this;
  }

  void swap(column_type& rhs) {
    using std::swap;
    swap(type_, rhs.type_);
    swap(bit_vector_length_, rhs.bit_vector_length_);
  }
  friend std::ostream& operator<<(std::ostream& os, const column_type& type) {
    os << type.type_ << type.bit_vector_length_;
    return os;
  }

 private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    int type = type_;
    ar & NAMED_MEMBER("type_", type)
      & MEMBER(bit_vector_length_);
    type_ = static_cast<type_name>(type);
  }
  friend class abstruct_column;
  type_name type_;
  int bit_vector_length_;
};

}  // namespace table
}  // namespace jubatus

#endif  // JUBATUS_TABLE_COLUMN_COLUMN_TYPE_HPP_
