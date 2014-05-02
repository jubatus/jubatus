// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_FRAMEWORK_LINEAR_MIXABLE_HPP_
#define JUBATUS_CORE_FRAMEWORK_LINEAR_MIXABLE_HPP_

#include <msgpack.hpp>
#include "jubatus/util/lang/shared_ptr.h"
#include "../common/exception.hpp"
#include "new_mixable.hpp"
#include "packer.hpp"

namespace jubatus {
namespace core {
namespace framework {

class diff_object_raw {
 public:
  virtual ~diff_object_raw();
  virtual void convert_binary(packer&) const = 0;
};

typedef jubatus::util::lang::shared_ptr<diff_object_raw> diff_object;

// public interface for Jubatus users
class linear_mixable : public virtual mixable {
 public:
  linear_mixable();
  virtual ~linear_mixable();

  virtual diff_object convert_diff_object(const msgpack::object&) const = 0;
  // mix assumes fold left?
  virtual void mix(const msgpack::object& obj, diff_object) const = 0;
  virtual void get_diff(packer&) const = 0;
  virtual bool put_diff(const diff_object& obj) = 0;
  virtual storage::version get_version() const;
};


}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_LINEAR_MIXABLE_HPP_
