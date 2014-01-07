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

#include "proxy_common.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "server_util.hpp"
#include "../common/membership.hpp"
#include "../common/signals.hpp"
#include "../common/util.hpp"

namespace jubatus {
namespace server {
namespace framework {

namespace {

std::string make_logfile_name(const proxy_argv& a) {
  std::ostringstream logfile;
  if (a.logdir != "") {
    logfile << a.logdir << '/';
    logfile << a.program_name << '.';
    logfile << a.eth << '_' << a.port;
    logfile << ".zklog.";
    logfile << getpid();
  }
  return logfile.str();
}

}  // namespace

proxy_common::proxy_common(const proxy_argv& a)
    : a_(a) {
  common::util::prepare_signal_handling();

  zk_.reset(common::create_lock_service(
      "cached_zk", a.z, a.zookeeper_timeout, make_logfile_name(a)));
  register_lock_service(zk_);
  jubatus::server::common::prepare_jubatus(*zk_, a_.type, "");

  if (a.daemon) {
    daemonize_process(a.logdir);
  }
}

proxy_common::~proxy_common() {
  close_lock_service();
}

void proxy_common::get_members_(
    const std::string& name, std::vector<std::pair<std::string, int> >& ret) {
  ret.clear();
  std::vector<std::string> list;
  std::string path;
  common::build_actor_path(path, a_.type, name);
  path += "/nodes";

  {
    jubatus::util::concurrent::scoped_lock lk(mutex_);
    zk_->list(path, list);
  }
  std::vector<std::string>::const_iterator it;

  if (list.empty()) {
    throw JUBATUS_EXCEPTION(no_worker(name));
  }

  // TODO(y-oda-oni-juba):
  // do you return all server list? it can be very large
  for (it = list.begin(); it != list.end(); ++it) {
    std::string ip;
    int port;
    common::revert(*it, ip, port);
    ret.push_back(make_pair(ip, port));
  }
}

void proxy_common::get_members_from_cht_(
    const std::string& name,
    const std::string& id,
    std::vector<std::pair<std::string, int> >& ret,
    size_t n) {
  ret.clear();
  jubatus::util::concurrent::scoped_lock lk(mutex_);
  jubatus::server::common::cht ht(zk_, a_.type, name);
  ht.find(id, ret, n);

  if (ret.empty()) {
    throw JUBATUS_EXCEPTION(no_worker(name));
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
