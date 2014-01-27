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

#ifndef JUBATUS_SERVER_COMMON_SIGNALS_HPP_
#define JUBATUS_SERVER_COMMON_SIGNALS_HPP_

#include "jubatus/util/lang/function.h"

namespace jubatus {
namespace server {
namespace common {

void prepare_signal_handling();  // NOTE: this function won't work well
                                 //   if you have any other threads.
                                 //   you should call this function
                                 //   at the head of program.
void set_action_on_term(jubatus::util::lang::function<void()> action);

}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_SIGNALS_HPP_
