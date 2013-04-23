// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_DRIVER_LINEAR_FUNCTION_MIXER_HPP_
#define JUBATUS_CORE_DRIVER_LINEAR_FUNCTION_MIXER_HPP_

#include "../framework/mixable.hpp"
#include "../storage/storage_base.hpp"

#include "diffv.hpp"

namespace jubatus {
namespace core {
namespace driver {

class linear_function_mixer : public jubatus::core::framework::mixable<
    storage::storage_base, diffv> {
 public:
  diffv get_diff_impl() const;

  void mix_impl(const diffv& lhs, const diffv& rhs, diffv& mixed) const;

  void put_diff_impl(const diffv& v);

  void clear();
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_LINEAR_FUNCTION_MIXER_HPP_
