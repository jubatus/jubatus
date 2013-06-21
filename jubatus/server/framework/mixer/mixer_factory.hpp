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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_FACTORY_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_FACTORY_HPP_

#include <pficommon/lang/shared_ptr.h>

#include "../../common/lock_service.hpp"
#include "../server_util.hpp"
#include "mixer.hpp"

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

mixer* create_mixer(const server_argv& a,
                    const pfi::lang::shared_ptr<common::lock_service>& zk);

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_FACTORY_HPP_
