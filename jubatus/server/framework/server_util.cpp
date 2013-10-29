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

#include "server_util.hpp"

#include <iostream>
#include <iomanip>
#include <string>

#include <glog/logging.h>
#include <pficommon/text/json.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/exception.hpp"
#include "../third_party/cmdline/cmdline.h"
#include "../common/config.hpp"
#include "../common/membership.hpp"
#include "../common/network.hpp"
#include "../common/util.hpp"

namespace jubatus {
namespace server {
namespace framework {

static const std::string VERSION(JUBATUS_VERSION);

namespace {
  const std::string IGNORED_TAG = "[IGNORED]";
  pfi::lang::shared_ptr<server::common::lock_service> ls;
}

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
    LOG(ERROR) << e.what();
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
  google::InitGoogleLogging(argv[0]);

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<std::string>("listen_addr", 'b', "bind IP address", false, "");
  p.add<std::string>("listen_if", 'B', "bind network interfance", false, "");
  p.add<int>("thread", 'c', "concurrency = thread number", false, 2);
  p.add<int>("timeout", 't', "time out (sec)", false, 10);
  p.add<std::string>("datadir", 'd', "directory to save and load models", false,
                     "/tmp");
  p.add<std::string>("logdir", 'l',
                     "directory to output logs (instead of stderr)", false, "");
  p.add<int, cmdline::range_reader<int> >(
      "loglevel", 'e', "verbosity of log messages", false, google::INFO,
      cmdline::range(google::INFO, google::FATAL));
  p.add<std::string>(
      "configpath",
      'f',
      "config option need to specify json file "
      "when standalone mode (without ZK mode)",
      false, "");
  p.add<std::string>("model_file", 'm',
                     "model data to load at startup", false, "");

  p.add<std::string>("zookeeper", 'z',
                     make_ignored_help("zookeeper location"), false);
  p.add<std::string>("name", 'n',
                     make_ignored_help("learning machine instance name"),
                     false);
  p.add<std::string>("mixer", 'x',
                     make_ignored_help("mixer strategy"), false, "");
  p.add("join", 'j', make_ignored_help("join to the existing cluster"));
  p.add<int>("interval_sec", 's',
             make_ignored_help("mix interval by seconds"), false, 16);
  p.add<int>("interval_count", 'i',
             make_ignored_help("mix interval by update count"), false, 512);
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
    print_version(common::util::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  bind_address = p.get<std::string>("listen_addr");
  bind_if = p.get<std::string>("listen_if");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  program_name = common::util::get_program_name();
  datadir = p.get<std::string>("datadir");
  logdir = p.get<std::string>("logdir");
  loglevel = p.get<int>("loglevel");
  configpath = p.get<std::string>("configpath");
  modelpath = p.get<std::string>("model_file");

  // determine listen-address and IPaddr used as ZK 'node-name'
  // TODO(y-oda-oni-juba): check bind_address is valid format
  if (!bind_address.empty()) {
    eth = bind_address;
  } else if (!bind_if.empty()) {
    bind_address = eth = common::util::get_ip(bind_if.c_str());
  } else {
    bind_address = "0.0.0.0";
    eth = jubatus::server::common::get_default_v4_address();
  }

#ifdef HAVE_ZOOKEEPER_H
  z = p.get<std::string>("zookeeper");
  name = p.get<std::string>("name");
  mixer = p.get<std::string>("mixer");
  join = p.exist("join");
  interval_sec = p.get<int>("interval_sec");
  interval_count = p.get<int>("interval_count");
  zookeeper_timeout = p.get<int>("zookeeper_timeout");
  interconnect_timeout = p.get<int>("interconnect_timeout");
#else
  z = "";
  name = "";
  join = false;
  interval_sec = 16;
  interval_count = 512;
#endif

  if (!is_standalone() && name.empty()) {
    std::cerr << "can't start multinode mode without name specified"
        << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (is_standalone() && configpath.empty()) {
    std::cerr << "can't start standalone mode without configpath specified"
        << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
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

  if ((!logdir.empty()) && (!common::util::is_writable(logdir.c_str()))) {
    std::cerr << "can't create log file: " << strerror(errno) << std::endl;
    exit(1);
  }
  set_log_destination(common::util::get_program_name());

#ifndef HAVE_ZOOKEEPER_H
  check_ignored_option(p, "zookeeper");
  check_ignored_option(p, "name");
  check_ignored_option(p, "mixer");
  check_ignored_option(p, "join");
  check_ignored_option(p, "interval_sec");
  check_ignored_option(p, "interval_count");
  check_ignored_option(p, "zookeeper_timeout");
  check_ignored_option(p, "interconnect_timeout");
#endif

  boot_message(common::util::get_program_name());
}

server_argv::server_argv()
    : join(false),
      port(9199),
      timeout(10),
      zookeeper_timeout(10),
      interconnect_timeout(10),
      threadnum(2),
      z(""),
      name(""),
      datadir("/tmp"),
      logdir(""),
      loglevel(google::INFO),
      eth("localhost"),
      interval_sec(5),
      interval_count(1024) {
}

void server_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth
      << ":" << port << '\n';
  ss << "    pid                  : " << getpid() << '\n';
  ss << "    user                 : " << common::util::get_user_name() << '\n';
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
  ss << "    loglevel             : " << google::GetLogSeverityName(loglevel)
      << '(' << loglevel << ')' << '\n';
#ifdef HAVE_ZOOKEEPER_H
  ss << "    zookeeper            : " << z << '\n';
  ss << "    name                 : " << name << '\n';
  ss << "    join                 : " << std::boolalpha << join << '\n';
  ss << "    interval sec         : " << interval_sec << '\n';
  ss << "    interval count       : " << interval_count << '\n';
  ss << "    zookeeper timeout    : " << zookeeper_timeout << '\n';
  ss << "    interconnect timeout : " << interconnect_timeout << '\n';
#endif
  LOG(INFO) << ss.str();
}

void server_argv::set_log_destination(const std::string& progname) const {
  if (logdir.empty()) {
    for (int severity = google::INFO; severity < google::NUM_SEVERITIES;
        severity++) {
      google::SetLogDestination(severity, "");
    }
    google::SetStderrLogging(loglevel);
  } else {
    for (int severity = google::INFO; severity < loglevel; severity++) {
      google::SetLogDestination(severity, "");
    }
    std::ostringstream basename, logdest, symlink;
    basename << progname << "." << eth << "_" << port;
    logdest << logdir << "/" << basename.str() << "."
        << (name.empty() ? "" : name + ".") << "log.";
    symlink << basename.str() << "." << getpid();
    google::SetLogDestination(loglevel, logdest.str().c_str());
    google::SetLogSymlink(loglevel, symlink.str().c_str());
    google::SetStderrLogging(google::FATAL);
  }
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
  google::InitGoogleLogging(argv[0]);

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<std::string>("listen_addr", 'b', "bind IP address", false, "");
  p.add<std::string>("listen_if", 'B', "bind network interfance", false, "");
  p.add<int>("thread", 'c', "concurrency = thread number", false, 16);
  p.add<int>("timeout", 't', "time out (sec)", false, 10);
  p.add<int>("zookeeper_timeout", 'Z', "zookeeper time out (sec)", false, 10);
  p.add<int>("interconnect_timeout", 'I',
      "interconnect time out between servers (sec)", false, 10);

  p.add<std::string>("zookeeper", 'z', "zookeeper location", false,
                     "localhost:2181");
  p.add<int>("pool_expire", 'E', "session-pool expire time (sec)", false, 60);
  p.add<int>("pool_size", 'S', "session-pool maximum size", false, 0);
  p.add<std::string>("logdir", 'l',
                     "directory to output logs (instead of stderr)", false, "");
  p.add<int, cmdline::range_reader<int> >(
      "loglevel", 'e', "verbosity of log messages", false, google::INFO,
      cmdline::range(google::INFO, google::FATAL));
  p.add("version", 'v', "version");

  p.parse_check(args, argv);

  if (p.exist("version")) {
    print_version(common::util::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  bind_address = p.get<std::string>("listen_addr");
  bind_if = p.get<std::string>("listen_if");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  zookeeper_timeout = p.get<int>("zookeeper_timeout");
  interconnect_timeout = p.get<int>("interconnect_timeout");
  program_name = common::util::get_program_name();
  z = p.get<std::string>("zookeeper");
  session_pool_expire = p.get<int>("pool_expire");
  session_pool_size = p.get<int>("pool_size");
  logdir = p.get<std::string>("logdir");
  loglevel = p.get<int>("loglevel");

  // determine listen-address and IPaddr used as ZK 'node-name'
  // TODO(y-oda-oni-juba): check bind_address is valid format
  if (!bind_address.empty()) {
    eth = bind_address;
  } else if (!bind_if.empty()) {
    bind_address = eth = common::util::get_ip(bind_if.c_str());
  } else {
    bind_address = "0.0.0.0";
    eth = jubatus::server::common::get_default_v4_address();
  }

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

  if ((!logdir.empty()) && (!common::util::is_writable(logdir.c_str()))) {
    std::cerr << "can't create log file: " << strerror(errno) << std::endl;
    exit(1);
  }
  set_log_destination(common::util::get_program_name());

  boot_message(common::util::get_program_name());
}

proxy_argv::proxy_argv()
    : port(9199),
      timeout(10),
      zookeeper_timeout(10),
      interconnect_timeout(10),
      threadnum(16),
      z("localhost:2181"),
      logdir(""),
      loglevel(google::INFO),
      eth("") {
}

void proxy_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth
      << ":" << port << '\n';
  ss << "    pid                  : " << getpid() << '\n';
  ss << "    user                 : " << common::util::get_user_name() << '\n';
  ss << "    timeout              : " << timeout << '\n';
  ss << "    zookeeper timeout    : " << zookeeper_timeout << '\n';
  ss << "    interconnect timeout : " << interconnect_timeout << '\n';
  ss << "    thread               : " << threadnum << '\n';
  ss << "    logdir               : " << logdir << '\n';
  ss << "    loglevel             : " << google::GetLogSeverityName(loglevel)
      << '(' << loglevel << ')' << '\n';
  ss << "    zookeeper            : " << z << '\n';
  LOG(INFO) << ss.str();
}

void proxy_argv::set_log_destination(const std::string& progname) const {
  if (logdir.empty()) {
    for (int severity = google::INFO; severity < google::NUM_SEVERITIES;
        severity++) {
      google::SetLogDestination(severity, "");
    }
    google::SetStderrLogging(loglevel);
  } else {
    for (int severity = google::INFO; severity < loglevel; severity++) {
      google::SetLogDestination(severity, "");
    }
    std::ostringstream basename, logdest, symlink;
    basename << progname << "." << eth << "_" << port;
    logdest << logdir << "/" << basename.str() << ".log.";
    symlink << basename.str() << "." << getpid();
    google::SetLogDestination(loglevel, logdest.str().c_str());
    google::SetLogSymlink(loglevel, symlink.str().c_str());
    google::SetStderrLogging(google::FATAL);
  }
}

void register_lock_service(pfi::lang::shared_ptr<common::lock_service> new_ls) {
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
