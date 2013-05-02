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

#ifndef JUBATUS_SERVER_FRAMEWORK_SERVER_HELPER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_SERVER_HELPER_HPP_

#include <sys/types.h>
#include <unistd.h>

#include <map>
#include <string>
#include <glog/logging.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/system/sysstat.h>

#include "jubatus/core/common/jsonconfig.hpp"
#include "mixer/mixer.hpp"
#include "server_util.hpp"
#include "../common/lock_service.hpp"
#include "../common/mprpc/rpc_server.hpp"
#include "../common/config.hpp"

namespace jubatus {
namespace server {
namespace framework {

class server_helper_impl {
 public:
  explicit server_helper_impl(const server_argv& a);
  void prepare_for_start(const server_argv& a, bool use_cht);
  void prepare_for_run(const server_argv& a, bool use_cht);
  void get_config_lock(const server_argv& a, int retry);

  pfi::lang::shared_ptr<common::lock_service> zk() const {
    return zk_;
  }

 private:
  pfi::lang::shared_ptr<common::lock_service> zk_;
  pfi::lang::shared_ptr<common::try_lockable> zk_config_lock_;
};

template<typename Server>
class server_helper {
 public:
  typedef typename Server::status_t status_t;

  explicit server_helper(const server_argv& a, bool use_cht = false)
      : impl_(a),
        use_cht_(use_cht) {
    impl_.prepare_for_start(a, use_cht);
    server_.reset(new Server(a, impl_.zk()));

    impl_.get_config_lock(a, 3);

    try {
      server_->set_config(get_conf(a));
    } catch (const core::jsonconfig::cast_check_error& e) {
      config_exception config_error;
      const core::jsonconfig::config_error_list& errors = e.errors();
      for (core::jsonconfig::config_error_list::const_iterator
          it = errors.begin(), end = errors.end(); it != end; ++it) {
        config_error << exception::error_message((*it)->what());
      }
      // send error message to caller
      throw JUBATUS_EXCEPTION(config_error);
    } catch (const pfi::lang::parse_error& e) {
      // exit immediately on JSON parse error with exit-code 1
      std::string msg =
          std::string("syntax error in configuration: ") +
          (a.is_standalone() ?
           a.configpath :
           std::string("<zookeeper>")) + ":" +
          pfi::lang::lexical_cast<std::string>(e.lineno()) + ":" +
          pfi::lang::lexical_cast<std::string>(e.pos()) + " " +
          e.msg();

      LOG(ERROR) << msg;
      exit(1);
    }
  }

  std::map<std::string, std::string> get_loads() const {
    std::map<std::string, std::string> result;
    {
      pfi::system::sysstat::sysstat_ret sys;
      get_sysstat(sys);
      result["loadavg"] = pfi::lang::lexical_cast<std::string>(sys.loadavg);
      result["total_memory"] = pfi::lang::lexical_cast<std::string>(
          sys.total_memory);
      result["free_memory"] = pfi::lang::lexical_cast<std::string>(
          sys.free_memory);
    }
    return result;
  }

  std::map<std::string, status_t> get_status() const {
    std::map<std::string, status_t> status;
    const server_argv& a = server_->argv();
    status_t& data = status[get_server_identifier(a)];

    util::machine_status_t mt;
    util::get_machine_status(mt);
    data["VIRT"] = pfi::lang::lexical_cast<std::string>(mt.vm_size);
    data["RSS"] = pfi::lang::lexical_cast<std::string>(mt.vm_resident);
    data["SHR"] = pfi::lang::lexical_cast<std::string>(mt.vm_share);

    // TBD: running_time, epoch_time
    // TBD: type(server type), name(instance name: when zookeeper enabled), eth
    data["timeout"] = pfi::lang::lexical_cast<std::string>(a.timeout);
    data["threadnum"] = pfi::lang::lexical_cast<std::string>(a.threadnum);
    data["datadir"] = a.datadir;
    data["interval_sec"] = pfi::lang::lexical_cast<std::string>(a.interval_sec);
    data["interval_count"] = pfi::lang::lexical_cast<std::string>(
        a.interval_count);
    data["is_standalone"] = pfi::lang::lexical_cast<std::string>(
        a.is_standalone());
    data["VERSION"] = JUBATUS_VERSION;
    data["PROGNAME"] = a.program_name;

    data["update_count"] = pfi::lang::lexical_cast<std::string>(
        server_->update_count());

    server_->get_status(data);

    server_->get_mixer()->get_status(data);
    data["zk"] = a.z;
    data["use_cht"] = pfi::lang::lexical_cast<std::string>(use_cht_);

    return status;
  }

  int start(common::mprpc::rpc_server& serv) {
    const server_argv& a = server_->argv();

    if (!a.is_standalone()) {
      server_->get_mixer()->start();
    }

    try {
      serv.listen(a.port, a.bind_address);
      LOG(INFO) << "start listening at port " << a.port;
      serv.start(a.threadnum, true);
      // RPC server started, then register group membership
      impl_.prepare_for_run(a, use_cht_);
      LOG(INFO) << jubatus::util::get_program_name() << " RPC server startup";
      serv.join();
      return 0;
    } catch (const mp::system_error& e) {
      if (e.code == EADDRINUSE) {
        LOG(FATAL) << "server failed to start: any process using port "
            << a.port << "?";
      } else {
        LOG(FATAL) << "server failed to start: " << e.what();
      }
    } catch (jubatus::exception::jubatus_exception&) {
      throw;
    } catch (const std::exception& e) {
      LOG(FATAL) << "server failed to start: " << e.what();
    }
    return -1;
  }

    pfi::lang::shared_ptr<Server> server() const {
    return server_;
  }

  pfi::concurrent::rw_mutex& rw_mutex() {
    return server_->rw_mutex();
  }

 private:
  pfi::lang::shared_ptr<Server> server_;
  server_helper_impl impl_;
  const bool use_cht_;
};

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#define JRLOCK_(p) \
  ::pfi::concurrent::scoped_rlock lk((p)->rw_mutex())

#define JWLOCK_(p) \
  ::pfi::concurrent::scoped_wlock lk((p)->rw_mutex()); \
  (p)->server()->event_model_updated()

#define NOLOCK_(p)

#endif  // JUBATUS_SERVER_FRAMEWORK_SERVER_HELPER_HPP_
