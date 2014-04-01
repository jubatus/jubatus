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

#include <iostream>
#include <list>
#include <string>
#include "keyword_filter_stage.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

keyword_filter_stage::keyword_filter_stage(const std::string& keyword)
  : keyword_(keyword),
    my_keyword_list_(util::my_keyword_list::get_instance()) {
}

bool keyword_filter_stage::is_drop(const window* w) const {
  std::list<std::string> l = my_keyword_list_->get_my_keywords();
  std::list<std::string>::iterator it;

  for (it = l.begin(); it != l.end(); ++it) {
    if (*it == keyword_) {
      return false;
    }
  }
  return true;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
