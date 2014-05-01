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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//USA

#ifndef JUBATUS_CORE_FRAMEWORK_PUSH_MIXABLE_HPP_
#define JUBATUS_CORE_FRAMEWORK_PUSH_MIXABLE_HPP_

#include "mixable.hpp"
#include "packer.hpp"
#include "../common/version.hpp"

namespace jubatus {
namespace core {
namespace framework {

// public interface for Jubatus users
class push_mixable : public virtual mixable {
 public:
  push_mixable();
  virtual ~push_mixable();

  virtual void get_argument(packer&) const = 0;
  virtual void pull(const msgpack::object& arg, packer&) const = 0;
  virtual void push(const msgpack::object&) = 0;
  //virtual storage::version get_version() const = 0;
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_PUSH_MIXABLE_HPP_
