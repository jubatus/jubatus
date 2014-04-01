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

#ifndef JUBATUS_CORE_BURST_WINDOW_KEYWORD_FILTER_STAGE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_KEYWORD_FILTER_STAGE_HPP_

#include <string>
#include "jubatus/util/lang/shared_ptr.h"
#include "stage_base.hpp"
#include "window.hpp"
#include "../my_keyword_list.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * the keyword filter stage.
 */
class keyword_filter_stage : public stage_base {
 public:
  /**
   * constructor
   * @param keyword keyword to filters
   */
  explicit keyword_filter_stage(const std::string& keyword);

 private:
  /**
   * the function to return whether or not
   * the window is dropped in this stage.
   * @param w the pointer of window
   * @return if keyword is not in my keyword list -> true,
   *         else                                 -> false
   */
  bool is_drop(const window* w) const;
  std::string keyword_;
  jubatus::util::lang::shared_ptr<util::my_keyword_list> my_keyword_list_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_SYSTEM_KEYWORD_FILTER_STAGE_HPP
