// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/core/common/version.hpp"
#include "jubatus/core/common/byte_buffer.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/core/framework/mixable_helper.hpp"
#include "jubatus/core/driver/driver.hpp"
#include "push_mixer.hpp"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::common::byte_buffer;

using std::cout;
using std::endl;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {
namespace {

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
  virtual bool hd_list(const std::string& path, std::string& out) {
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
};

}  // namespace

TEST(push_communication, update_members) {
  jubatus::util::lang::shared_ptr<common::lock_service> zk(new zk_stub());
  const pair<string, int> my_id(std::make_pair("127.0.0.1", 1112));
  jubatus::util::lang::shared_ptr<push_communication> com =
    push_communication::create(zk, "test_type", "test_name", 1, my_id);

  ASSERT_EQ(3, com->update_members());  // must not be 4
  vector<pair<string, int> > list = com->servers_list();
  ASSERT_EQ(3, list.size());
  for (size_t i = 0; i < list.size(); ++i) {
    ASSERT_NE(list[i], my_id);  // must not include "127.0.0.1:1112"
  }
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
