// Jubatus: Online machine learning framework for distributed environment
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

#include <vector>

#include "batch.hpp"
#include "batch_weight_reset_stage.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

bool batch_weight_reset_stage::is_drop(const window* w) const {
  return false;
}

void batch_weight_reset_stage::do_something(window* w) {
  const std::vector<double> default_weight_vec(
      w->get_batch_size(),
      kDefaultBatchWeight);
  w->set_batch_weights(default_weight_vec);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
