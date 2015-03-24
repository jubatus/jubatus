// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "jubatus/core/common/exception.hpp"
#include "server_util.hpp"
#include "../common/logger/logger.hpp"
#include "../common/membership.hpp"
#include "../common/signals.hpp"

using jubatus::util::system::time::clock_time;
using jubatus::util::system::time::get_clock_time;

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
    : a_(a),
      request_counter_(0),
      forward_counter_(0),
      start_time_(get_clock_time()) {
  common::prepare_signal_handling();

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
  path += "/actives";

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

void proxy_common::update_request_counter() {
  jubatus::util::concurrent::scoped_lock lk(mutex_);
  ++request_counter_;
}

void proxy_common::update_forward_counter(const uint64_t count) {
  jubatus::util::concurrent::scoped_lock lk(mutex_);
  forward_counter_ += count;
}

proxy_common::status_type proxy_common::get_status() {
  update_request_counter();

  status_type status;
  string_map& data = status[get_proxy_identifier(a_)];

  const clock_time ct = get_clock_time();
  data["clock_time"] =
      jubatus::util::lang::lexical_cast<std::string>(ct.sec);
  data["start_time"] =
      jubatus::util::lang::lexical_cast<std::string>(start_time_.sec);
  data["uptime"] =
      jubatus::util::lang::lexical_cast<std::string>((ct - start_time_).sec);

  common::machine_status_t mt;
  common::get_machine_status(mt);
  data["VIRT"] =
      jubatus::util::lang::lexical_cast<std::string>(mt.vm_size);
  data["RSS"] =
      jubatus::util::lang::lexical_cast<std::string>(mt.vm_resident);
  data["SHR"] =
      jubatus::util::lang::lexical_cast<std::string>(mt.vm_share);

  data["VERSION"] = JUBATUS_VERSION;
  data["PROGNAME"] = a_.program_name;
  data["pid"] =
      jubatus::util::lang::lexical_cast<std::string>(getpid());
  data["user"] = jubatus::server::common::get_user_name();

  data["threadnum"] =
      jubatus::util::lang::lexical_cast<std::string>(a_.threadnum);
  data["timeout"] =
      jubatus::util::lang::lexical_cast<std::string>(a_.timeout);

  data["logdir"] = a_.logdir;
  data["log_config"] = a_.log_config;

  data["zookeeper"] = a_.z;
  data["connected_zookeeper"] = zk_->get_connected_host_and_port();
  data["zookeeper_timeout"] =
      jubatus::util::lang::lexical_cast<std::string>(a_.zookeeper_timeout);
  data["interconnect_timeout"] =
      jubatus::util::lang::lexical_cast<std::string>
      (a_.interconnect_timeout);

  data["session_pool_expire"] =
      jubatus::util::lang::lexical_cast<std::string>(a_.session_pool_expire);
  data["session_pool_size"] =
      jubatus::util::lang::lexical_cast<std::string>(a_.session_pool_size);

  data["request_count"] =
      jubatus::util::lang::lexical_cast<std::string>(request_counter_);
  data["forward_count"] =
      jubatus::util::lang::lexical_cast<std::string>(forward_counter_);

  return status;
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
