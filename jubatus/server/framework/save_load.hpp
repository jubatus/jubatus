// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_SAVE_LOAD_HPP_
#define JUBATUS_SERVER_FRAMEWORK_SAVE_LOAD_HPP_

#include <stdio.h>
#include <string>
#include <iostream>

#include "server_base.hpp"

namespace jubatus {
namespace server {
namespace framework {

void save_server(
    FILE* fp,
    const server_base& server,
    const std::string& id);
void load_server(
    std::istream& is,
    server_base& server,
    const std::string& id,
    bool overwrite_config);

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_SAVE_LOAD_HPP_
