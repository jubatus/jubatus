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

#ifndef JUBATUS_SERVER_COMMON_CONFIG_HPP_
#define JUBATUS_SERVER_COMMON_CONFIG_HPP_

#include <string>
#include <vector>
#include <map>
#include <pficommon/text/json.h>
#include <glog/logging.h>

#include "lock_service.hpp"
#include "membership.hpp"

namespace jubatus {
namespace server {
namespace common {

void config_fromlocal(const std::string& path, std::string& config);
#ifdef HAVE_ZOOKEEPER_H
void config_fromzk(
    lock_service& z,
    const std::string& type,
    const std::string& name,
    std::string& config);

void config_tozk(
    lock_service& z,
    const std::string& type,
    const std::string& name,
    std::string& config);

void config_tozk(
    lock_service& z,
    const std::string& type,
    const std::string& name,
    std::string& config,
    const std::string& config_src);

void remove_config_fromzk(
    lock_service& z,
    const std::string& type,
    const std::string&);

bool is_no_workers(
    lock_service& z,
    const std::string& type,
    const std::string& name);
#endif

}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_CONFIG_HPP_
