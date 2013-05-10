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

#include <cassert>
#include <string>

#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/exception.hpp"
#include "global_id_generator_base.hpp"
#include "global_id_generator_zk.hpp"

namespace jubatus {
namespace server {
namespace common {

global_id_generator_zk::global_id_generator_zk() {
}

global_id_generator_zk::~global_id_generator_zk() {
}

void global_id_generator_zk::set_ls(
    pfi::lang::shared_ptr<lock_service>& ls,
    const std::string& path_prefix) {
  path_ = path_prefix + "/id_generator";
  ls_ = ls;
  if (!ls_->create(path_)) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
        "Failed to create global id generator")
        << jubatus::exception::error_api_func("lock_service::create")
        << jubatus::exception::error_message(path_));
  }
}

uint64_t global_id_generator_zk::generate() {
  uint64_t res;
  if ( !ls_ ) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("lock_service is not given"));
  }
  if (ls_->create_id(path_, 0, res)) {
    return res;
  } else {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to create id"));
  }
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
