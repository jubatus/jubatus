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

#include "burst_system_builder.hpp"

#include <string>

namespace jubatus {
namespace core {
namespace burst {
namespace system {

burst_system* burst_system_builder::build(const std::string& keyword_txt,
                                          double scaling_param,
                                          double gamma) {
  return new burst_system(keyword_txt,
                          window_batch_size_,
                          batch_interval_,
                          scaling_param,
                          gamma,
                          costcut_threshold_,
                          max_reuse_batch_num_,
                          result_window_rotate_size_);
}

void burst_system_builder::delete_ptr(burst_system* s) {
  delete s;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
