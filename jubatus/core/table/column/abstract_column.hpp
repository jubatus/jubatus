// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012,2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301	USA


#ifndef JUBATUS_CORE_TABLE_COLUMN_ABSTRUCT_COLUMN_HPP_
#define JUBATUS_CORE_TABLE_COLUMN_ABSTRUCT_COLUMN_HPP_
#include <memory>
#include <iterator>
#include <msgpack.hpp>
#include <pficommon/lang/demangle.h>
#include "../../common/assert.hpp"
#include "../storage_exception.hpp"
#include "bit_vector.hpp"
#include "column_type.hpp"

namespace jubatus {
namespace core {
namespace table {

namespace detail {

class abstract_column_base {
public:
  abstract_column_base(const column_type& type)
    :my_type_(type) {}
	virtual ~abstract_column_base() {}
	column_type type() const {
		return my_type_;
	}
	template <typename U>
	void push_back(const U& type) {
    throw type_unmatch_exception(
      "column: invalid type push_back(): " + pfi::lang::get_typename<U>());
	}
	template <typename U>
	bool insert(uint64_t target, const U& value) {
    throw type_unmatch_exception(
      "column: invalid type insert(): " + pfi::lang::get_typename<U>());
	}
	template <typename U>
	bool update(uint64_t target, const U& value) {
    throw type_unmatch_exception(
      "column: invalid type update(): " + pfi::lang::get_typename<U>());
		return true;
  }
  virtual bool remove(uint64_t target) = 0;
  virtual void clear() = 0;
  virtual void pack_with_index(const uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
  }
	virtual void dump() const = 0;
	virtual void dump(std::ostream& os, uint64_t target) const = 0;
private:
	column_type my_type_;
};

}	 // namespace detail

template <typename T>
class typed_column : public detail::abstract_column_base {
public:
  typed_column(const column_type& type)
    :detail::abstract_column_base(type) { }

  using detail::abstract_column_base::push_back;
  using detail::abstract_column_base::insert;
  using detail::abstract_column_base::update;

	void push_back(const T& type) {
    array_.push_back(type);
	}
  void push_back(const msgpack::object obj) {
    array_.push_back(T());
    obj.convert(&array_[array_.size()]);
  }
	bool insert(uint64_t target, const T& value) {
    if (size() < target) {
      return false;
    }
    array_.insert(std::advance(array_.begin(), target), value);
    return true;
	}
	bool update(uint64_t index, const T& value) {
    if (size() < index) {
      return false;
    }
		array_[index] = value;
    return true;
	}

  bool update(uint64_t index, const msgpack::object obj) {
    if (size() < index) {
      return false;
    }
    obj.convert(&array_[index]);
    return true;
  }

	bool remove(uint64_t target) {
    if (size() < target) {
      return false;
    }
    std::swap(array_[target], array_.back());
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
    if (size() < index) {
      throw length_unmatch_exception(
        "invalid index [" + pfi::lang::lexical_cast<std::string>(index) +
        "] for [" +
        pfi::lang::lexical_cast<std::string>(array_.size()));
    }
    return array_[index];
  }

  T& operator[](uint64_t index) {
    if (size() < index) {
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
  typed_column(const column_type& type)
    :detail::abstract_column_base(type) {
  }

  using detail::abstract_column_base::push_back;
  using detail::abstract_column_base::insert;
  using detail::abstract_column_base::update;

  void push_back(const msgpack::object obj) {
    array_.push_back(uint64_t());
    obj.convert(&array_[array_.size()]);
  }
  bool update(uint64_t index, const msgpack::object obj) {
    if (size() < index) {
      return false;
    }
    obj.convert(&array_[index]);
    return true;
  }

  uint64_t size() const {
    return 1;
  }
  bit_vector operator[](uint64_t index) {
		return bit_vector(
      &array_[bit_vector::memory_size(type().bit_vector_length()) * index],
      type().bit_vector_length());
  }
  bit_vector operator[](uint64_t index) const {
		return bit_vector(
      &array_[bit_vector::memory_size(type().bit_vector_length()) * index],
      type().bit_vector_length());
  }
  bool remove(uint64_t target) {
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

/*
  class bit_vector_column : detail::abstract_column {
  public:
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

	bool remove(uint64_t target) {
  if (size() <= target) {
  std::cout << "size:" << size() << " <= target:" << target << std::endl;
  return false;
  }
  const uint64_t memory_size = bit_vector::memory_size(vector_bit_num_);
  char* const from = ptr_ + (target + 1) * memory_size;
  char* const to = ptr_ + target * memory_size;
  std::memmove(to, from, size_ - (target + 1) * memory_size);
  size_ -= memory_size;
  return true;
	}

	void dump() const {
  std::cout << "[column (bit_vector)"
  << " size:" << size() << " {" << std::endl;
  for (size_t i = 0; i <	size(); ++i) {
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
  for (size_t i = 0; i <	bv.size(); ++i) {
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
*/

namespace detail {
class abstract_column {
public:
  abstract_column(const column_type& type) {
    if (type.is(column_type::uint8_type)) {
    base_ =
      pfi::lang::shared_ptr<detail::abstract_column_base>(new uint8_column(type));
    } else if (type.is(column_type::uint16_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new uint16_column(type));
    } else if (type.is(column_type::uint32_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new uint32_column(type));
    } else if (type.is(column_type::uint64_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new uint64_column(type));
    } else if (type.is(column_type::int8_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new int8_column(type));
    } else if (type.is(column_type::int16_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new int16_column(type));
    } else if (type.is(column_type::int32_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new int32_column(type));
    } else if (type.is(column_type::int64_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new int64_column(type));
    } else if (type.is(column_type::float_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new float_column(type));
    } else if (type.is(column_type::double_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new double_column(type));
    } else if (type.is(column_type::string_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new string_column(type));
    } else if (type.is(column_type::bit_vector_type)) {
      base_ =
        pfi::lang::shared_ptr<detail::abstract_column_base>(new bit_vector_column(type));
    }
  }
  column_type type() const {
    return base_->type();
  }

	template <typename T>
	void push_back(const T& value) {
    const column_type& type = base_->type();
    if (type.is(column_type::uint8_type)) {
      uint8_column& target(*static_cast<uint8_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::uint16_type)) {
      uint16_column& target(*static_cast<uint16_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::uint32_type)) {
      uint32_column& target(*static_cast<uint32_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::uint64_type)) {
      uint64_column& target(*static_cast<uint64_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::int8_type)) {
      int8_column& target(*static_cast<int8_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::int16_type)) {
      int16_column& target(*static_cast<int16_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::int32_type)) {
      int32_column& target(*static_cast<int32_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::int64_type)) {
      int64_column& target(*static_cast<int64_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::float_type)) {
      float_column& target(*static_cast<float_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::double_type)) {
      double_column& target(*static_cast<double_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::string_type)) {
      string_column& target(*static_cast<string_column*>(base_.get()));
      target.push_back(value);
    } else if (type.is(column_type::bit_vector_type)) {
      bit_vector_column& target(*static_cast<bit_vector_column*>(base_.get()));
      target.push_back(value);
    }
	}

	template <typename T>
	bool insert(uint64_t index, const T& value) {
    const column_type& type = base_->type();
    if (type.is(column_type::uint8_type)) {
      uint8_column& target(*static_cast<uint8_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::uint16_type)) {
      uint16_column& target(*static_cast<uint16_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::uint32_type)) {
      uint32_column& target(*static_cast<uint32_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::uint64_type)) {
      uint64_column& target(*static_cast<uint64_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::int8_type)) {
      int8_column& target(*static_cast<int8_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::int16_type)) {
      int16_column& target(*static_cast<int16_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::int32_type)) {
      int32_column& target(*static_cast<int32_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::int64_type)) {
      int64_column& target(*static_cast<int64_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::float_type)) {
      float_column& target(*static_cast<float_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::double_type)) {
      double_column& target(*static_cast<double_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::string_type)) {
      string_column& target(*static_cast<string_column*>(base_.get()));
      target.insert(index, value);
    } else if (type.is(column_type::bit_vector_type)) {
      bit_vector_column& target(*static_cast<bit_vector_column*>(base_.get()));
      target.insert(index, value);
    }
    return true;
	}
	template <typename T>
	bool update(uint64_t index, const T& value) {
    const column_type& type = base_->type();
    if (type.is(column_type::uint8_type)) {
      uint8_column& target(*static_cast<uint8_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::uint16_type)) {
      uint16_column& target(*static_cast<uint16_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::uint32_type)) {
      uint32_column& target(*static_cast<uint32_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::uint64_type)) {
      uint64_column& target(*static_cast<uint64_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::int8_type)) {
      int8_column& target(*static_cast<int8_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::int16_type)) {
      int16_column& target(*static_cast<int16_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::int32_type)) {
      int32_column& target(*static_cast<int32_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::int64_type)) {
      int64_column& target(*static_cast<int64_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::float_type)) {
      float_column& target(*static_cast<float_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::double_type)) {
      double_column& target(*static_cast<double_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::string_type)) {
      string_column& target(*static_cast<string_column*>(base_.get()));
      target.update(index, value);
    } else if (type.is(column_type::bit_vector_type)) {
      bit_vector_column& target(*static_cast<bit_vector_column*>(base_.get()));
      target.update(index, value);
    }
		return true;
  }
  bool remove(uint64_t index) {
    const column_type& type = base_->type();
    if (type.is(column_type::uint8_type)) {
      uint8_column& target(*static_cast<uint8_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::uint16_type)) {
      uint16_column& target(*static_cast<uint16_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::uint32_type)) {
      uint32_column& target(*static_cast<uint32_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::uint64_type)) {
      uint64_column& target(*static_cast<uint64_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::int8_type)) {
      int8_column& target(*static_cast<int8_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::int16_type)) {
      int16_column& target(*static_cast<int16_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::int32_type)) {
      int32_column& target(*static_cast<int32_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::int64_type)) {
      int64_column& target(*static_cast<int64_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::float_type)) {
      float_column& target(*static_cast<float_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::double_type)) {
      double_column& target(*static_cast<double_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::string_type)) {
      string_column& target(*static_cast<string_column*>(base_.get()));
      target.remove(index);
    } else if (type.is(column_type::bit_vector_type)) {
      bit_vector_column& target(*static_cast<bit_vector_column*>(base_.get()));
      target.remove(index);
    }
    return true;
  };
  virtual void clear() {
  }
  virtual void pack_with_index(const uint64_t index, msgpack::packer<msgpack::sbuffer>& pk) const {
  }
  abstract_column_base* get() {
    return base_.get();
  }
  const abstract_column_base* get() const {
    return base_.get();
  }

	void dump() const {}
	void dump(std::ostream& os, uint64_t target) const {
  }
private:
  pfi::lang::shared_ptr<abstract_column_base> base_;
};
}

}	 // namespace table
}	 // namespace core
}	 // namespace jubatus

#endif	// JUBATUS_CORE_TABLE_COLUMN_ABSTRUCT_COLUMN_HPP_
