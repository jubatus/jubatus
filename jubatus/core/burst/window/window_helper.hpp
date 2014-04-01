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

/**
 * @file window_helper.hpp
 */
#ifndef JUBATUS_CORE_BURST_WINDOW_WINDOW_HELPER_HPP_
#define JUBATUS_CORE_BURST_WINDOW_WINDOW_HELPER_HPP_

#include "window.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

window* create_window_ptr();

window* create_window_ptr(const double& pos,
                          const int32_t window_batch_size,
                          const int32_t batch_interval);

window* create_window_ptr(const window& w);

void delete_window_ptr(window* w);
void delete_window_ptr(const window* w);

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_WINDOW_HELPER_HPP_
