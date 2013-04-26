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

#ifndef JUBATUS_SERVER_FRAMEWORK_SERVER_BASE_HPP_
#define JUBATUS_SERVER_FRAMEWORK_SERVER_BASE_HPP_

#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/framework/mixable.hpp"
#include "server_util.hpp"

namespace jubatus {
namespace server {
namespace framework {

namespace mixer {
class mixer;
}  // namespace mixer

class server_base {
 public:
  typedef std::map<std::string, std::string> status_t;

  explicit server_base(const server_argv& a);
  virtual ~server_base() {}

  virtual mixer::mixer* get_mixer() const = 0;
  virtual pfi::lang::shared_ptr<core::framework::mixable_holder>
      get_mixable_holder() const = 0;
  virtual void get_status(status_t& status) const = 0;

  virtual bool save(const std::string& id);
  virtual bool load(const std::string& id);
  void event_model_updated();

  uint64_t update_count() const {
    return update_count_;
  }

  pfi::concurrent::rw_mutex& rw_mutex() {
    return get_mixable_holder()->rw_mutex();
  }

  const server_argv& argv() const {
    return argv_;
  }

 private:
  const server_argv argv_;
  uint64_t update_count_;
};

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_SERVER_BASE_HPP_
