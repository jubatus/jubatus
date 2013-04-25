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

#ifndef JUBATUS_CORE_STORAGE_NORM_BASE_HPP_
#define JUBATUS_CORE_STORAGE_NORM_BASE_HPP_

#include <string>

namespace jubatus {
namespace core {
namespace storage {

class norm_base {
 public:
  norm_base() {
  }

  virtual ~norm_base() {
  }
  virtual void clear() = 0;
  virtual void notify(const std::string& row, float old_val, float new_val) = 0;
  virtual float calc_norm(const std::string& row) const = 0;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_NORM_BASE_HPP_
