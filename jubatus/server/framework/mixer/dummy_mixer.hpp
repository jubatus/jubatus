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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_DUMMY_MIXER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_DUMMY_MIXER_HPP_

#include <vector>
#include "jubatus/core/framework/mixable.hpp"
#include "mixer.hpp"

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

class dummy_mixer : public mixer {
 public:
  void register_api(rpc_server_t& server) {
  }
  void set_mixable_holder(
      pfi::lang::shared_ptr<core::framework::mixable_holder>) {
  }

  void start() {
  }
  void stop() {
  }

  void updated() {
  }

  void get_status(server_base::status_t& status) const {
  }
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_DUMMY_MIXER_HPP_
