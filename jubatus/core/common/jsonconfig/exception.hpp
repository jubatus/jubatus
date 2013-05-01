// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_JSONCONFIG_EXCEPTION_HPP_
#define JUBATUS_CORE_COMMON_JSONCONFIG_EXCEPTION_HPP_

#include <string>
#include <stdexcept>
#include <typeinfo>
#include <vector>

#include <pficommon/text/json.h>
#include <pficommon/lang/shared_ptr.h>

#include "../exception.hpp"

namespace jubatus {
namespace core {
namespace jsonconfig {

class config_error : public exception::jubaexception<config_error> {
 public:
  config_error(const std::string& path, const std::string& message);

  ~config_error() throw ();

  const std::string& path() const {
    return path_;
  }

  const char* what() const throw () {
    return message_.c_str();
  }

 private:
  const std::string path_;
  const std::string message_;
};

class type_error : public config_error {
 public:
  type_error(
      const std::string& path,
      pfi::text::json::json::json_type_t expect,
      pfi::text::json::json::json_type_t actual);

  ~type_error() throw ();

  pfi::text::json::json::json_type_t expect() const {
    return expect_;
  }

  pfi::text::json::json::json_type_t actual() const {
    return actual_;
  }

 private:
  const pfi::text::json::json::json_type_t expect_;
  const pfi::text::json::json::json_type_t actual_;
};

class out_of_range : public config_error {
 public:
  out_of_range(const std::string& path, size_t size, size_t index);

  ~out_of_range() throw ();

  size_t size() const {
    return size_;
  }

  size_t position() const {
    return index_;
  }

 private:
  size_t size_;
  size_t index_;
};

class not_found : public config_error {
 public:
  not_found(const std::string& path, const std::string& key);

  ~not_found() throw ();

  const std::string& key() const {
    return key_;
  }

 private:
  std::string key_;
};

// cast_check_error DOES NOT INHERIT Config_error
class cast_check_error : public exception::jubaexception<cast_check_error> {
 public:
  cast_check_error(
      const std::vector<pfi::lang::shared_ptr<config_error> >& errors);

  ~cast_check_error() throw ();

  size_t size() const {
    return errors_.size();
  }

  const std::vector<pfi::lang::shared_ptr<config_error> >& errors() const {
    return errors_;
  }

 private:
  std::vector<pfi::lang::shared_ptr<config_error> > errors_;
};

}  // namespace jsonconfig
}  // namespace core
}  // namespace jubatus
#endif  // JUBATUS_CORE_COMMON_JSONCONFIG_EXCEPTION_HPP_
