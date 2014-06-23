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
#include "jubatus/util/system/time_util.h"
#include "jubatus/util/concurrent/rwmutex.h"
#include "jubatus/util/lang/shared_ptr.h"

#include "jubatus/core/driver/driver.hpp"
#include "server_util.hpp"

using jubatus::util::system::time::clock_time;

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

  virtual core::driver::driver_base* get_driver() const = 0;
  virtual void get_status(status_t& status) const = 0;
  virtual void set_config(const std::string& config) = 0;

  virtual bool clear();
  virtual bool save(const std::string& id);
  virtual bool load(const std::string& id);

  void load_file(const std::string& path);
  void event_model_updated();
  void update_saved_status(const std::string& path);
  void update_loaded_status(const std::string& path);

  virtual std::string get_config() const = 0;
  virtual uint64_t user_data_version() const = 0;

  uint64_t update_count() const {
    return update_count_;
  }

  jubatus::util::concurrent::rw_mutex& rw_mutex() {
    return rw_mutex_;
  }

  const server_argv& argv() const {
    return argv_;
  }

  uint64_t last_saved_sec() const {
    return last_saved_.sec;
  }

  const std::string& last_saved_path() const {
    return last_saved_path_;
  }

  uint64_t last_loaded_sec() const {
    return last_loaded_.sec;
  }

  const std::string& last_loaded_path() const {
    return last_loaded_path_;
  }

 private:
  const server_argv argv_;
  uint64_t update_count_;
  clock_time last_saved_;
  std::string last_saved_path_;
  clock_time last_loaded_;
  std::string last_loaded_path_;
  jubatus::util::concurrent::rw_mutex rw_mutex_;
};

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_SERVER_BASE_HPP_
