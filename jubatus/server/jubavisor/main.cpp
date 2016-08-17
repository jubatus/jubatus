// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <locale.h>

#include <iostream>
#include <string>

#include "jubatus/util/lang/bind.h"

#include "jubatus/core/common/exception.hpp"
#include "../third_party/cmdline/cmdline.h"
#include "../common/logger/logger.hpp"
#include "../common/signals.hpp"
#include "../common/filesystem.hpp"

#include "jubavisor.hpp"

namespace {
const std::string PROGNAME = "jubavisor";
}  // namespace

using jubatus::server::common::set_action_on_term;
using jubatus::server::jubavisor::jubavisor;
using jubatus::server::jubavisor::jubavisor_server;
using jubatus::util::lang::_1;
using jubatus::util::lang::_2;
using jubatus::util::lang::_3;
using jubatus::util::lang::bind;
using jubatus::util::lang::ref;

namespace {

bool logger_configured_ = false;

void stop_on_term(jubavisor_server& serv) {
  LOG(INFO) << "stopping RPC server";
  serv.rpc_end();
}

void configure_logger(const std::string& log_config) {
  if (log_config.empty()) {
    if (!logger_configured_) {
      jubatus::server::common::logger::configure();
    }
  } else {
    if (logger_configured_) {
      LOG(INFO) << "reloading log configuration: " << log_config;
    }
    jubatus::server::common::logger::configure(log_config);
  }

  if (!jubatus::server::common::logger::is_configured()) {
    std::cerr << "failed to configure logger" << std::endl;
    ::exit(1);
  }

  logger_configured_ = true;
  jubatus::server::common::set_action_on_hup(jubatus::util::lang::bind(
      configure_logger, jubatus::util::lang::ref(log_config)));
}

}  // namespace

int main(int argc, char* argv[]) try {
  // Explicitly set the current locale to support Unicode output in logs.
  ::setlocale(LC_ALL, "");

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9198,
             cmdline::range(1, 65535));
  p.add<std::string>("zookeeper", 'z', "zookeeper location", true);
  p.add<std::string>("logdir", 'l', "log to output all child process' log",
                     false, "/tmp");
  p.add<std::string>("log_config", 'g',
                     "log4cxx XML configuration file", false, "");
  p.add<int>("timeout", 't', "rpc timeout", false, 10);
  p.add("daemon", 'd', "daemonize the process");
  p.parse_check(argc, argv);
  int port = p.get<int>("rpc-port");
  std::string logfile = "";
  std::string log_config = p.get<std::string>("log_config");

  // Configure the logger.
  if (!log_config.empty()) {
    log_config = jubatus::server::common::real_path(log_config);
  }
  jubatus::server::common::logger::setup_parameters(
      jubatus::server::common::get_program_name().c_str(), "", port);
  configure_logger(log_config);

  if (p.exist("daemon")) {
    jubatus::server::common::append_server_path(argv[0]);
    logfile = p.get<std::string>("logdir") + "/" + PROGNAME + ".";

    int r = jubatus::server::common::daemonize();

    if (r == -1) {
      LOG(ERROR) << "failed at daemon(3): errno=" << errno;
      return -1;
    }
    logfile += "zklog";
    LOG(INFO) << "starting with daemon mode on port " << port;
    LOG(INFO) << "writing ZooKeeper logs to " << logfile;
  } else {
    LOG(INFO) << " starting with interactive mode on port " << port;
  }

  jubavisor j(p.get<std::string>("zookeeper"), port, 16, logfile);
  jubavisor_server serv(10 * 1024);
  {
    serv.set_start(bind(&jubavisor::start, &j, _1, _2, _3));
    serv.set_stop(bind(&jubavisor::stop, &j, _1, _2));

    try {
      serv.rpc_listen(port);
      serv.rpc_start(2, true);
      set_action_on_term(bind(&stop_on_term, ref(serv)));
      serv.rpc_join();
    } catch (const mp::system_error& e) {
      if (e.code == EADDRINUSE) {
        LOG(ERROR) << "server failed to start: " << e.what()
                   << " (any process using port " << port << "?)";
      } else {
        LOG(FATAL) << "server failed to start: " << e.what();
      }
      return -1;
    } catch (const std::exception& e) {
      LOG(FATAL) << "error when starting RPC server: " << e.what();
    }
  }
  return 0;
} catch (const jubatus::core::common::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}
