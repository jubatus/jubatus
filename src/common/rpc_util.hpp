// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once
#include <string>
#include <pficommon/data/serialization.h>
#include <msgpack.hpp>

namespace jubatus {

typedef std::pair<std::string, int> connection_info;

template <typename T, typename E = std::string>
struct result {
  bool success;
  T retval;
  E error;

  static result<T,E> ok(const T& t){
    result<T,E> r;
    r.success = true;
    r.retval = t;
    return r;
  };
  static result<T,E> fail(const E& e){
    result<T,E> r;
    r.success = false;
    r.error = e;
    return r;
  };

  MSGPACK_DEFINE(success, retval, error);
  template <class Archiver>
  void serialize(Archiver &ar) {
    ar & MEMBER(success) & MEMBER(retval) & MEMBER(error);
  }
};

} //namespace jubatus
