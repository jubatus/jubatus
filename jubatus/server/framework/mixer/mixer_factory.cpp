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

#include <pficommon/lang/shared_ptr.h>

#include "mixer_factory.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include "linear_mixer.hpp"
#else
#include "dummy_mixer.hpp"
#endif

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

mixer* create_mixer(const server_argv& a,
                    const pfi::lang::shared_ptr<common::lock_service>& zk) {
#ifdef HAVE_ZOOKEEPER_H
  return new linear_mixer(
      linear_communication::create(zk, a.type, a.name, a.timeout),
      a.interval_count, a.interval_sec);
#else
  return new dummy_mixer;
#endif
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
