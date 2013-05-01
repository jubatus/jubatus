// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_EXCEPTION_INFO_HPP_
#define JUBATUS_CORE_COMMON_EXCEPTION_INFO_HPP_

#include <cstring>
#include <string>
#include <typeinfo>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/demangle.h>

namespace jubatus {
namespace exception {

class error_info_base {
 public:
  virtual bool splitter() const {
    return false;
  }

  virtual std::string tag_typeid_name() const = 0;
  virtual std::string as_string() const = 0;

  virtual ~error_info_base() throw () {
  }
};

template<class Tag, class V>
class error_info;

template<class Tag, class V>
inline std::string to_string(const error_info<Tag, V>& info) {
  return pfi::lang::lexical_cast<std::string, V>(info.value());
}

template<>
class error_info<struct error_splitter_, void> : public error_info_base {
 public:
  bool splitter() const {
    return true;
  }

  std::string tag_typeid_name() const {
    return pfi::lang::demangle(typeid(struct error_splitter_*).name());
  }

  std::string as_string() const {
    // USE splitter or tag_typeid_name
    return "-splitter-";
  }
};

template<class Tag, class V>
class error_info : public error_info_base {
 public:
  typedef V value_type;
  explicit error_info(value_type v);
  ~error_info() throw ();

  std::string tag_typeid_name() const;
  std::string as_string() const;

  value_type value() const {
    return value_;
  }

 private:
  value_type value_;
};

template<class Tag, class V>
inline error_info<Tag, V>::error_info(value_type v)
    : value_(v) {
}

template<class Tag, class V>
inline error_info<Tag, V>::~error_info() throw () {
}

template<class Tag, class V>
inline std::string error_info<Tag, V>::tag_typeid_name() const {
  return pfi::lang::demangle(typeid(Tag*).name());
}

template<class Tag, class V>
inline std::string error_info<Tag, V>::as_string() const {
  return to_string(*this);
}

}  // namespace exception
}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_EXCEPTION_INFO_HPP_
