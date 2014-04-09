// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
//// This library is free software; you can redistribute it and/or
//// modify it under the terms of the GNU Lesser General Public
//// License version 2.1 as published by the Free Software Foundation.
////
//// This library is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//// Lesser General Public License for more details.
////
//// You should have received a copy of the GNU Lesser General Public
//// License along with this library; if not, write to the Free Software
//// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//USA

#ifndef JUBATUS_CORE_FRAMEWORK_MODEL_HPP_
#define JUBATUS_CORE_FRAMEWORK_MODEL_HPP_

#include "packer.hpp"

namespace jubatus {
namespace core {
namespace framework {

// public interface for Jubatus users
class model {
 public:
  virtual ~model();
  // TODO(unknown): Replace pack/unpack arguments
  //virtual void save(packer&) const = 0;
  //virtual void load(const msgpack::object&) = 0;
  virtual void pack(msgpack::packer<msgpack::sbuffer>& packer) const = 0;
  virtual void unpack(msgpack::object o) = 0;
  virtual void clear();
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_MODEL_HPP_
