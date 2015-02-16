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

#include "server_util.hpp"

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include <string>

#include "jubatus/util/text/json.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/lang/bind.h"

#include "jubatus/core/common/exception.hpp"
#include "../third_party/cmdline/cmdline.h"
#include "../common/config.hpp"
#include "../common/filesystem.hpp"
#include "../common/logger/logger.hpp"
#include "../common/membership.hpp"
#include "../common/network.hpp"
#include "../common/system.hpp"
#include "../common/signals.hpp"

namespace jubatus {
namespace server {
namespace framework {

static const std::string VERSION(JUBATUS_VERSION);

namespace {

const std::string IGNORED_TAG = "[IGNORED]";
jubatus::util::lang::shared_ptr<server::common::lock_service> ls;
bool logger_configured_ = false;

struct lower_bound_reader {
  explicit lower_bound_reader(int l)
    : low(l) {}
  int operator()(const std::string& s) const {
    int ret = cmdline::default_reader<int>()(s);
    if (ret < low) {
      throw
        cmdline::cmdline_error("value should be more than " +
                               cmdline::detail::lexical_cast<std::string>(low));
    }
    return ret;
  }
 private:
  int low;
};

void configure_logger(const std::string& log_config) {
  if (log_config.empty()) {
    common::logger::configure();
  } else {
    if (logger_configured_) {
      LOG(INFO) << "reloading log configuration: " << log_config;
    }
    common::logger::configure(log_config);
  }

  if (!common::logger::is_configured()) {
    std::cerr << "failed to configure logger" << std::endl;
    ::exit(1);
  }

  logger_configured_ = true;
  common::set_action_on_hup(jubatus::util::lang::bind(
      configure_logger, jubatus::util::lang::ref(log_config)));
}

}  // namespace

void print_version(const std::string& progname) {
  std::cout << "jubatus-" << VERSION << " (" << progname << ")" << std::endl;
}

std::string get_conf(const server_argv& a) {
  config_json conf;

  try {
    if (a.is_standalone()) {
      conf.load_json(a.configpath);
    } else {
#ifdef HAVE_ZOOKEEPER_H
      conf.load_json(a.z, a.type, a.name);
#endif
    }
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(ERROR) << "exception when loading config file: "
               << e.diagnostic_information(true);
    exit(1);
  }
  return conf.config;
}

void config_json::load_json(
    const std::string& zkhosts,
    const std::string& type,
    const std::string& name) {
#ifdef HAVE_ZOOKEEPER_H
  LOG(INFO) << "load config from zookeeper: " << zkhosts;
  jubatus::server::common::config_fromzk(*ls, type, name, config);
#endif
}

void config_json::load_json(const std::string& filepath) {
  LOG(INFO) << "load config from local file: " << filepath;
  jubatus::server::common::config_fromlocal(filepath, config);
}

void check_ignored_option(const cmdline::parser& p, const std::string& key) {
  if (p.exist(key)) {
    LOG(WARNING) << "\"" << key << "\" option is ignored";
  }
}

std::string make_ignored_help(const std::string& help) {
#ifdef HAVE_ZOOKEEPER_H
  return help;
#else
  return IGNORED_TAG + " " + help;
#endif
}

server_argv::server_argv(int args, char** argv, const std::string& type)
    : type(type) {
  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199,
             cmdline::range(1, 65535));
  p.add<std::string>("listen_addr", 'b', "bind IP address", false, "");
  p.add<std::string>("listen_if", 'B', "bind network interfance", false, "");
  p.add<int>("thread", 'c', "concurrency = thread number", false, 2,
             lower_bound_reader(1));
  p.add<int>("timeout", 't', "time out (sec)", false, 10,
             lower_bound_reader(0));
  p.add<std::string>("datadir", 'd', "directory to save and load models", false,
                     "/tmp");
  p.add<std::string>("logdir", 'l',
                     "directory to output ZooKeeper logs (instead of stderr)",
                     false, "");
  p.add<std::string>("log_config", 'g',
                     "log4cxx XML configuration file", false, "");
  p.add<std::string>(
      "configpath",
      'f',
      "config option need to specify json file "
      "when standalone mode (without ZK mode)",
      false, "");
  p.add<std::string>("model_file", 'm',
                     "model data to load at startup", false, "");
  p.add("daemon", 'D', "launch in daemon mode (ignores SIGHUP)");

  p.add<std::string>("zookeeper", 'z',
                     make_ignored_help("zookeeper location"), false);
  p.add<std::string>("name", 'n',
                     make_ignored_help("learning machine instance name"),
                     false);
  p.add<std::string>("mixer", 'x',
                     make_ignored_help("mixer strategy"), false,
                     "linear_mixer");
  p.add<int>("interval_sec", 's',
             make_ignored_help("mix interval by seconds"), false, 16,
             lower_bound_reader(0));
  p.add<int>("interval_count", 'i',
             make_ignored_help("mix interval by update count"), false, 512,
             lower_bound_reader(0));
  p.add<int>("zookeeper_timeout", 'Z',
             make_ignored_help("zookeeper time out (sec)"), false, 10);
  p.add<int>("interconnect_timeout", 'I',
             make_ignored_help("interconnect time out between servers (sec)"),
             false, 10);

  // APPLY CHANGES TO JUBAVISOR WHEN ARGUMENTS MODIFIED

  p.add("version", 'v', "version");

#ifndef HAVE_ZOOKEEPER_H
  p.footer("\nAll " + IGNORED_TAG +
           " options are for compatibility with distributed mode");
#endif

  p.parse_check(args, argv);

  if (p.exist("version")) {
    print_version(common::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  bind_address = p.get<std::string>("listen_addr");
  bind_if = p.get<std::string>("listen_if");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  program_name = common::get_program_name();
  datadir = p.get<std::string>("datadir");
  logdir = p.get<std::string>("logdir");
  log_config = p.get<std::string>("log_config");
  configpath = p.get<std::string>("configpath");
  modelpath = p.get<std::string>("model_file");
  daemon = p.exist("daemon");

  // determine listen-address and IPaddr used as ZK 'node-name'
  // TODO(y-oda-oni-juba): check bind_address is valid format
  if (!bind_address.empty()) {
    eth = bind_address;
  } else if (!bind_if.empty()) {
    bind_address = eth = common::get_ip(bind_if.c_str());
  } else {
    bind_address = "0.0.0.0";
    eth = jubatus::server::common::get_default_v4_address();
  }

  // Configure the logger.
  if (!log_config.empty()) {
    log_config = common::real_path(log_config);
  }
  common::logger::setup_parameters(
      common::get_program_name().c_str(), eth.c_str(), port);
  configure_logger(log_config);

#ifdef HAVE_ZOOKEEPER_H
  z = p.get<std::string>("zookeeper");
  name = p.get<std::string>("name");
  mixer = p.get<std::string>("mixer");
  interval_sec = p.get<int>("interval_sec");
  interval_count = p.get<int>("interval_count");
  zookeeper_timeout = p.get<int>("zookeeper_timeout");
  interconnect_timeout = p.get<int>("interconnect_timeout");
#else
  z = "";
  name = "";
  interval_sec = 16;
  interval_count = 512;
#endif

  if (!is_standalone() && name.empty()) {
    std::cerr << "can't start multinode mode without name specified"
        << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (is_standalone()) {
    if (configpath.empty()) {
      std::cerr << "can't start standalone mode without configpath specified"
          << std::endl;
      std::cerr << p.usage() << std::endl;
      exit(1);
    }
    configpath = common::real_path(configpath);
  }

  if (!modelpath.empty()) {
    modelpath = common::real_path(modelpath);
  }

  if (!is_standalone() && zookeeper_timeout < 1) {
    std::cerr << "can't start with zookeeper_timeout less than 1" << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (!is_standalone() && interconnect_timeout < 1) {
    std::cerr << "can't start with interconnect_timeout less than 1"
      << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (!datadir.empty()) {
    datadir = common::real_path(datadir);
    if (!common::is_writable(datadir.c_str())) {
      std::cerr << "can't use datadir: " << strerror(errno) << std::endl;
      std::cerr << p.usage() << std::endl;
      exit(1);
    }
  }

  if (!logdir.empty()) {
    logdir = common::real_path(logdir);
    if (!common::is_writable(logdir.c_str())) {
      std::cerr << "can't write to the ZooKeeper log directory: "
                << strerror(errno) << std::endl;
      exit(1);
    }
  }

#ifndef HAVE_ZOOKEEPER_H
  check_ignored_option(p, "zookeeper");
  check_ignored_option(p, "name");
  check_ignored_option(p, "mixer");
  check_ignored_option(p, "interval_sec");
  check_ignored_option(p, "interval_count");
  check_ignored_option(p, "zookeeper_timeout");
  check_ignored_option(p, "interconnect_timeout");
#endif

  boot_message(common::get_program_name());
}

server_argv::server_argv()
    : port(9199),
      timeout(10),
      zookeeper_timeout(10),
      interconnect_timeout(10),
      threadnum(2),
      z(""),
      name(""),
      datadir("/tmp"),
      logdir(""),
      log_config(""),
      eth("localhost"),
      interval_sec(5),
      interval_count(1024) {
}

void server_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth
      << ":" << port << '\n';
  ss << "    pid                  : " << getpid() << '\n';
  ss << "    user                 : " << common::get_user_name() << '\n';
  ss << "    mode                 : ";
  if (is_standalone()) {
    ss << "standalone mode\n";
  } else {
    ss << "multinode mode\n";
  }
  ss << "    timeout              : " << timeout << '\n';
  ss << "    thread               : " << threadnum << '\n';
  ss << "    datadir              : " << datadir << '\n';
  ss << "    logdir               : " << logdir << '\n';
  ss << "    log config           : " << log_config << '\n';
#ifdef HAVE_ZOOKEEPER_H
  ss << "    zookeeper            : " << z << '\n';
  ss << "    name                 : " << name << '\n';
  if (0 < interval_sec) {
    ss << "    interval sec         : " << interval_sec << '\n';
  } else {
    ss << "    interval sec         : disabled" << '\n';
  }
  if (0 < interval_count) {
    ss << "    interval count       : " << interval_count << '\n';
  } else {
    ss << "    interval count       : disabled" << '\n';
  }
  ss << "    zookeeper timeout    : " << zookeeper_timeout << '\n';
  ss << "    interconnect timeout : " << interconnect_timeout << '\n';
#endif
  LOG(INFO) << ss.str();
}

void daemonize_process(const std::string& logdir) {
  if (logdir == "" && ::isatty(::fileno(stderr))) {
    LOG(WARNING) << "logs may be lost because started "
                 << "in daemon mode without log directory";
  }
  if (::signal(SIGHUP, SIG_IGN) == SIG_ERR) {
    LOG(FATAL) << "Failed to ignore SIGHUP";
  }
  LOG(INFO) << "set daemon mode (SIGHUP is now ignored)";
}

std::string get_server_identifier(const server_argv& a) {
  std::stringstream ss;
  ss << a.eth;
  ss << "_";
  ss << a.port;
  return ss.str();
}

proxy_argv::proxy_argv(int args, char** argv, const std::string& t)
    : type(t) {
  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199,
             cmdline::range(1, 65535));
  p.add<std::string>("listen_addr", 'b', "bind IP address", false, "");
  p.add<std::string>("listen_if", 'B', "bind network interfance", false, "");
  p.add<int>("thread", 'c', "concurrency = thread number", false, 4,
             lower_bound_reader(1));
  p.add<int>("timeout", 't', "time out (sec)", false, 10,
             lower_bound_reader(0));
  p.add<int>("zookeeper_timeout", 'Z', "zookeeper time out (sec)", false, 10);
  p.add<int>("interconnect_timeout", 'I',
      "interconnect time out between servers (sec)", false, 10);
  p.add("daemon", 'D', "launch in daemon mode (ignores SIGHUP)");

  p.add<std::string>("zookeeper", 'z', "zookeeper location", false,
                     "localhost:2181");
  p.add<int>("pool_expire", 'E', "session-pool expire time (sec)", false, 60,
             lower_bound_reader(0));
  p.add<int>("pool_size", 'S', "session-pool maximum size", false, 0,
             lower_bound_reader(0));
  p.add<std::string>("logdir", 'l',
                     "directory to output ZooKeeper logs (instead of stderr)",
                     false, "");
  p.add<std::string>("log_config", 'g',
                     "log4cxx XML configuration file", false, "");
  p.add("version", 'v', "version");

  p.parse_check(args, argv);

  if (p.exist("version")) {
    print_version(common::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  bind_address = p.get<std::string>("listen_addr");
  bind_if = p.get<std::string>("listen_if");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  zookeeper_timeout = p.get<int>("zookeeper_timeout");
  interconnect_timeout = p.get<int>("interconnect_timeout");
  daemon = p.exist("daemon");
  program_name = common::get_program_name();
  z = p.get<std::string>("zookeeper");
  session_pool_expire = p.get<int>("pool_expire");
  session_pool_size = p.get<int>("pool_size");
  logdir = p.get<std::string>("logdir");
  log_config = p.get<std::string>("log_config");

  // determine listen-address and IPaddr used as ZK 'node-name'
  // TODO(y-oda-oni-juba): check bind_address is valid format
  if (!bind_address.empty()) {
    eth = bind_address;
  } else if (!bind_if.empty()) {
    bind_address = eth = common::get_ip(bind_if.c_str());
  } else {
    bind_address = "0.0.0.0";
    eth = jubatus::server::common::get_default_v4_address();
  }

  // Configure the logger.
  if (!log_config.empty()) {
    log_config = common::real_path(log_config);
  }
  common::logger::setup_parameters(
      common::get_program_name().c_str(), eth.c_str(), port);
  configure_logger(log_config);

  if (zookeeper_timeout < 1) {
    std::cerr << "can't start with zookeeper_timeout less than 1" << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (interconnect_timeout < 1) {
    std::cerr << "can't start with interconnect_timeout less than 1"
      << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if ((!logdir.empty()) && (!common::is_writable(logdir.c_str()))) {
    std::cerr << "can't create log file: " << strerror(errno) << std::endl;
    exit(1);
  }

  boot_message(common::get_program_name());
}

proxy_argv::proxy_argv()
    : port(9199),
      timeout(10),
      zookeeper_timeout(10),
      interconnect_timeout(10),
      threadnum(4),
      z("localhost:2181"),
      logdir(""),
      log_config(""),
      eth("") {
}

void proxy_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth
      << ":" << port << '\n';
  ss << "    pid                  : " << getpid() << '\n';
  ss << "    user                 : " << common::get_user_name() << '\n';
  ss << "    timeout              : " << timeout << '\n';
  ss << "    zookeeper timeout    : " << zookeeper_timeout << '\n';
  ss << "    interconnect timeout : " << interconnect_timeout << '\n';
  ss << "    thread               : " << threadnum << '\n';
  ss << "    logdir               : " << logdir << '\n';
  ss << "    log config           : " << log_config << '\n';
  ss << "    zookeeper            : " << z << '\n';
  LOG(INFO) << ss.str();
}

std::string get_proxy_identifier(const proxy_argv& a) {
  std::stringstream ss;
  ss << a.eth;
  ss << "_";
  ss << a.port;
  return ss.str();
}

void register_lock_service(
    jubatus::util::lang::shared_ptr<common::lock_service> new_ls) {
  if (ls) {
    throw JUBATUS_EXCEPTION(
        jubatus::core::common::exception::runtime_error(
            "lock service is already registered"));
  }
  ls.swap(new_ls);
#ifdef HAVE_ZOOKEEPER_H
  if (ls) {
    ::atexit(&close_lock_service);
  }
#endif
}

void close_lock_service() {
#ifdef HAVE_ZOOKEEPER_H
  if (ls) {
    ls->force_close();
    ls.reset();
  }
#endif
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
