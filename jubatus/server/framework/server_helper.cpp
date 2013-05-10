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

#include "server_helper.hpp"

#include <string>

#include "../common/cht.hpp"
#include "../common/membership.hpp"
#include "../common/util.hpp"

namespace jubatus {
namespace server {
namespace framework {

using std::string;
using std::ostringstream;

namespace {

string make_logfile_name(const server_argv& a) {
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

server_helper_impl::server_helper_impl(const server_argv& a) {
#ifdef HAVE_ZOOKEEPER_H
  if (!a.is_standalone()) {
    zk_.reset(common::create_lock_service("zk",
                                          a.z, a.timeout,
                                          make_logfile_name(a)));
  }
#endif
}

void server_helper_impl::prepare_for_start(const server_argv& a, bool use_cht) {
#ifdef HAVE_ZOOKEEPER_H
  if (!a.is_standalone()) {
    ls = zk_;
    common::prepare_jubatus(*zk_, a.type, a.name);

    if (a.join) {  // join to the existing cluster with -j option
      LOG(INFO) << "joining to the cluseter " << a.name;
      LOG(ERROR) << "join is not supported yet :(";
    }
  }
#endif
}

void server_helper_impl::prepare_for_run(const server_argv& a, bool use_cht) {
#ifdef HAVE_ZOOKEEPER_H
  if (!a.is_standalone()) {
    ls = zk_;

    if (use_cht) {
      common::cht::setup_cht_dir(*zk_, a.type, a.name);
      common::cht ht(zk_, a.type, a.name);
      ht.register_node(a.eth, a.port);
    }

    register_actor(*zk_, a.type, a.name, a.eth, a.port);
    LOG(INFO) << "registered group membership";
  }
#endif
}

void server_helper_impl::get_config_lock(const server_argv& a, int retry) {
#ifdef HAVE_ZOOKEEPER_H
  if (!a.is_standalone()) {
    string lock_path;
    common::build_config_lock_path(lock_path, a.type, a.name);
    zk_config_lock_ = pfi::lang::shared_ptr<common::try_lockable>(
        new common::lock_service_mutex(*zk_, lock_path));

    while (!zk_config_lock_->try_rlock()) {
      if (retry == 0) {
        throw JUBATUS_EXCEPTION(
            jubatus::exception::runtime_error("any user is writing config?"));
      }
      retry--;
      sleep(1);
    }
  }
#endif
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
