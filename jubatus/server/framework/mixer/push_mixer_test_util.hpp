// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2015 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_PUSH_MIXER_TEST_UTIL_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_PUSH_MIXER_TEST_UTIL_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/core/common/version.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/core/framework/mixable_helper.hpp"
#include "jubatus/core/driver/driver.hpp"
#include "push_mixer.hpp"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

class zk_stub : public common::lock_service {
  string stub;

 public:
  zk_stub() {}
  ~zk_stub() {}
  void force_close() {}
  bool create(
      const std::string& path,
      const std::string& payload = "",
      bool ephemeral = false) {
    return true;
  }
  virtual bool set(
      const std::string& path,
      const std::string& payload = "") {
    return true;
  }
  virtual bool remove(const std::string& path) {
    return true;
  }
  virtual bool exists(const std::string& path) {
    return true;
  }

  virtual bool bind_watcher(
      const std::string& path,
      jubatus::util::lang::function<void(int, int, std::string)>&) {
    return true;
  }

  virtual bool bind_child_watcher(
      const std::string& path,
      const jubatus::util::lang::function<void(int, int, std::string)>&) {
    return true;
  }


  virtual bool bind_delete_watcher(
      const std::string& path,
      jubatus::util::lang::function<void(std::string)>&) {
    return true;
  }

  // ephemeral only
  virtual bool create_seq(const std::string& path, std::string&) {
    return true;
  }
  virtual bool create_id(
      const std::string& path,
      uint32_t prefix,
      uint64_t& res) {
    return true;
  }

  virtual bool list(const std::string& path, std::vector<std::string>& out) {
    out.push_back("127.0.0.1_1111");
    out.push_back("127.0.0.1_1112");
    out.push_back("127.0.0.1_1113");
    out.push_back("127.0.0.1_1114");
    return true;
  }

  // reads data (should be smaller than 1024B)
  virtual bool read(const std::string& path, std::string& out) {
    return true;
  }

  virtual void push_cleanup(const jubatus::util::lang::function<void()>& f) {
  }
  virtual void run_cleanup() {
  }

  virtual const std::string& get_hosts() const {
    return stub;
  }
  virtual const std::string type() const {
    return "";
  }
  virtual const std::string get_connected_host_and_port() const {
    return "";
  }
  virtual void reopen_logfile() {
  }
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_PUSH_MIXER_TEST_UTIL_HPP_
