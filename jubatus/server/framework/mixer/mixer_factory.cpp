// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixer_factory.hpp"

#include <string>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/core/common/exception.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include "linear_mixer.hpp"
#include "random_mixer.hpp"
#include "broadcast_mixer.hpp"
#include "skip_mixer.hpp"
#else
#include "dummy_mixer.hpp"
#endif

using std::make_pair;
using std::string;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

mixer* create_mixer(
    const server_argv& a,
    const jubatus::util::lang::shared_ptr<common::lock_service>& zk,
    jubatus::util::concurrent::rw_mutex& model_mutex) {
#ifdef HAVE_ZOOKEEPER_H
  const string& use_mixer = a.mixer;
  if (use_mixer == "linear_mixer") {
    return new linear_mixer(
        linear_communication::create(
            zk,
            a.type,
            a.name,
            a.interconnect_timeout,
            std::make_pair(a.eth, a.port)),
        model_mutex,
        a.interval_count, a.interval_sec);
  } else if (use_mixer == "random_mixer") {
    return new random_mixer(
        push_communication::create(
            zk,
            a.type,
            a.name,
            a.interconnect_timeout),
        model_mutex,
        a.interval_count, a.interval_sec, make_pair(a.eth, a.port));
  } else if (use_mixer == "broadcast_mixer") {
    return new broadcast_mixer(
        push_communication::create(zk, a.type, a.name, a.interconnect_timeout),
        model_mutex,
        a.interval_count, a.interval_sec, make_pair(a.eth, a.port));
  } else if (use_mixer == "skip_mixer") {
    return new skip_mixer(
        push_communication::create(zk, a.type, a.name, a.interconnect_timeout),
        model_mutex,
        a.interval_count, a.interval_sec, make_pair(a.eth, a.port));
  } else {
    throw JUBATUS_EXCEPTION(jubatus::core::common::exception::runtime_error(
          "unsupported mix type (" + use_mixer + ")"));
  }
#else
  return new dummy_mixer;
#endif
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
