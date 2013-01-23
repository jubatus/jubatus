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

#include "keeper_common.hpp"

#include <iostream>
#include <glog/logging.h>

#include "../common/membership.hpp"
#include "../common/exception.hpp"
#include "../common/util.hpp"
#include "server_util.hpp"

using namespace jubatus;
using namespace jubatus::framework;

namespace {

std::string make_logfile_name(const keeper_argv& a) {
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

}

keeper_common::keeper_common(const keeper_argv &a)
    : a_(a),
      zk_(common::create_lock_service("cached_zk", a.z, a.timeout,
                                      make_logfile_name(a))) {
  ls = zk_;
  jubatus::common::prepare_jubatus(*zk_, a_.type, "");
}

keeper_common::~keeper_common() {
}

void keeper_common::get_members_(
    const std::string& name, std::vector<std::pair<std::string, int> >& ret) {
  using namespace std;
  ret.clear();
  vector < string > list;
  string path;
  common::build_actor_path(path, a_.type, name);
  path += "/nodes";

  {
    pfi::concurrent::scoped_lock lk(mutex_);
    zk_->list(path, list);
  }
  vector<string>::const_iterator it;

  if (list.empty()) {
    throw JUBATUS_EXCEPTION(no_worker(name));
  }

  // FIXME:
  // do you return all server list? it can be very large
  for (it = list.begin(); it != list.end(); ++it) {
    string ip;
    int port;
    common::revert(*it, ip, port);
    ret.push_back(make_pair(ip, port));
  }
}

void keeper_common::get_members_from_cht_(
    const std::string& name, const std::string& id,
    std::vector<std::pair<std::string, int> >& ret, size_t n) {
  ret.clear();
  pfi::concurrent::scoped_lock lk(mutex_);
  jubatus::common::cht ht(zk_, a_.type, name);
  ht.find(id, ret, n);

  if (ret.empty()) {
    throw JUBATUS_EXCEPTION(no_worker(name));
  }
}
