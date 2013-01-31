// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "exception.hpp"
#include <cstring>
#include <string>
#include <pficommon/lang/demangle.h>

namespace jubatus {
namespace exception {

error_info_list_t jubatus_exception::error_info() const {
  return info_list_;
}

std::string jubatus_exception::diagnostic_information(bool display_what) const {
  std::ostringstream tmp;

  tmp << "Dynamic exception type: ";
  tmp << pfi::lang::demangle(typeid(*this).name());

  if (display_what && strcmp(what(), "")) {
    tmp << "::what: " << what();
  }

  tmp << '\n';

  size_t frame = 0;
  for (error_info_list_t::const_iterator it = info_list_.begin(), end =
      info_list_.end(); it != end; ++it) {
    if ((*it)->splitter()) {
      frame++;
      continue;
    }
    tmp << "    #" << frame << " [" << (*it)->tag_typeid_name() << "] = "
        << (*it)->as_string() << '\n';
  }
  return tmp.str();
}

}  // namespace exception
}  // namespace jubatus
