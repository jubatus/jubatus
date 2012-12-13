// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#pragma once
#include "lock_service.hpp"
#include "membership.hpp"

#include <string>
#include <vector>
#include <map>
#include <pficommon/text/json.h>

namespace jubatus {
namespace common {

void config_fromlocal(const std::string&, std::string&);
#ifdef HAVE_ZOOKEEPER_H
void config_fromzk(lock_service& , const std::string& , const std::string& , std::string&);
void config_tozk(lock_service& , const std::string& , const std::string& , std::string&);
#endif

} // common
} // jubatus
