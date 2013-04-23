// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_JUBAVISOR_PROCESS_HPP_
#define JUBATUS_SERVER_JUBAVISOR_PROCESS_HPP_

#include <string>

#include "../framework/server_util.hpp"

namespace jubatus {
namespace server {
namespace jubavisor {

// child process
class process {
 public:
  explicit process(const std::string& zkhosts);
  process(
      const std::string& zkhosts,
      const framework::server_argv& server_option);
  ~process();

  bool set_names(const std::string& str);
  bool spawn_link(int p);
  bool kill();

  bool has_samespec(const process& rhs) const;

  const std::string& get_name() const {
    return name_;
  }
  const std::string& get_server() const {
    return server_;
  }
  int get_rpc_port() const {
    return server_option_.port;
  }
  int get_pid() const {
    return pid_;
  }

 private:
  std::string server_;
  std::string name_;
  std::string zk_hosts_;
  framework::server_argv server_option_;
  int pid_;
};

}  // namespace jubavisor
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_JUBAVISOR_PROCESS_HPP_
