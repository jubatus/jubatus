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

#ifndef JUBATUS_CORE_COMMON_JSONCONFIG_CONFIG_HPP_
#define JUBATUS_CORE_COMMON_JSONCONFIG_CONFIG_HPP_

#include <stdint.h>
#include <string>
#include <typeinfo>
#include <utility>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>

#include "exception.hpp"

namespace jubatus {
namespace core {
namespace jsonconfig {

class config;

template<class T>
T config_cast(const config& c);

class config {
 public:
  class iterator;

  config()
      : json_() {
  }

  explicit config(const pfi::text::json::json& j)
      : json_(j) {
  }

  config(const pfi::text::json::json& j, const std::string& path)
      : json_(j),
        path_(path) {
  }

  template<typename T>
  T As() const {
    return config_cast<T>(*this);
  }

  config operator[](size_t index) const;

  config operator[](const std::string& key) const;

  bool contain(const std::string& key) const;

  iterator begin() const;
  iterator end() const;

  size_t size() const;
  const pfi::text::json::json& get() const {
    return json_;
  }
  const std::string& path() const {
    return path_;
  }

  template<class T>
  bool is() const {
    return pfi::text::json::is<T>(json_);
  }

  pfi::text::json::json::json_type_t type() const {
    return json_.type();
  }

  class iterator {  // const_iterator
   public:
    typedef pfi::text::json::json::const_iterator iterator_base;
    iterator(const iterator&);
    iterator(
        const config& parent,
        const pfi::text::json::json::const_iterator& it);

    const std::string& key() const;
    config value() const;

    // InputIterator
    bool operator==(const iterator& it) const {
      return it_ == it.it_;
    }

    bool operator!=(const iterator& it) const {
      return !(*this == it);
    }

    std::pair<const std::string, pfi::text::json::json> operator*() const {
      return *it_;
    }

    const std::pair<const std::string, pfi::text::json::json>* operator->()
        const {
      return it_.operator->();
    }
    // FowrardIterator
    const iterator& operator++() {
      ++it_;
      return *this;
    }
    const iterator operator++(int /* unused */) {
      iterator temp(*this);
      ++it_;
      return temp;
    }

   private:
    const config& parent_;
    pfi::text::json::json::const_iterator it_;
  };

 private:
  pfi::text::json::json json_;
  std::string path_;
};

}  // namespace jsonconfig
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_JSONCONFIG_CONFIG_HPP_
