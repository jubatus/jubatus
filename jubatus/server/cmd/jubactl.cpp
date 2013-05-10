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

#include <string>
#include <vector>
#include <iostream>
#include <cstring>

#include <glog/logging.h>
#include <jubatus/msgpack/rpc/client.h>
#include <pficommon/lang/function.h>

#include "jubatus/core/common/exception.hpp"
#include "../third_party/cmdline/cmdline.h"
#include "../common/zk.hpp"
#include "../common/membership.hpp"
#include "../framework/server_util.hpp"

static const std::string PROGNAME(JUBATUS_APPNAME "ctl");
static const std::string VERSION(JUBATUS_VERSION);

using std::string;
using std::cout;
using std::endl;
using std::vector;

void usage();
void send2supervisor(
    const string& cmd,
    const string& type,
    const string& name,
    const string& zkhosts,
    const cmdline::parser& argv);
void send2server(
    const string& cmd,
    const string& name,
    const string& zkhosts);
void status(const string& type, const string& name, const string& zkhosts);

int main(int argc, char** argv)
try {
  cmdline::parser p;

  p.add<std::string>("cmd", 'c',
      "command to send servers (start|stop|save|load|status)", true);
  p.add<std::string>("server", 's',
      "server exec file of learning machine (jubaclassifier, ...)", true);
  p.add<std::string>("name", 'n', "learning machine name", true);
  p.add<std::string>("type", 't',
      "learning machine type (classifier, ...)", true);
  p.add<unsigned int>("num", 'N',
      "num of process in the whole cluster (one on each server when 0)", false);
  p.add<std::string>("zookeeper", 'z',
      "ZooKeeper location; environment variable 'ZK' is used when available",
      false);

  // Support framework::server_argv
  p.add<std::string>("listen_if", 'B',
      "[start] bind network interfance", false, "");
  p.add<int>("thread", 'C', "[start] concurrency = thread number", false, 2);
  p.add<int>("timeout", 'T', "[start] time out (sec)", false, 10);
  p.add<std::string>("datadir", 'D',
      "[start] directory to load and save models", false, "/tmp");
  p.add<std::string>("logdir", 'L',
      "[start] directory to output logs (instead of stderr)", false, "");
  p.add<int, cmdline::range_reader<int> >("loglevel", 'E',
      "[start] verbosity of log messages", false, google::INFO,
      cmdline::range(google::INFO, google::FATAL));
  p.add("join", 'J', "[start] join to the existing cluster");
  p.add<int>("interval_sec", 'S', "[start] mix interval by seconds", false, 16);
  p.add<int>("interval_count", 'I',
      "[start] mix interval by update count", false, 512);

  p.add("debug", 'd', "debug mode");

  p.parse_check(argc, argv);

  google::InitGoogleLogging(argv[0]);
  if (p.exist("debug")) {
    google::LogToStderr();  // only when debug
  }

  string cmd = p.get<std::string>("cmd");
  string name = p.get<std::string>("server") + "/" + p.get<std::string>("name");
  string type = p.get<std::string>("type");

  string zk;
  char* zkenv = std::getenv("ZK");

  if (p.get<std::string>("zookeeper") != "") {
    zk = p.get<std::string>("zookeeper");
  } else if (zkenv) {
    zk = zkenv;
  } else {
    cout << "can't get ZK location: "
        << "set 'ZK' environment or specify '-z <somezkaddrs>" << endl;
    cout << p.get<std::string>("zookeeper") << endl;
    cout << p.usage() << endl;
    exit(1);
  }

  if (cmd == "status") {
    status(type, p.get<string>("name"), zk);
    return 0;
  }

  if (cmd == "start" or cmd == "stop") {
    send2supervisor(cmd, type, name, zk, p);
  } else if (cmd == "save" or cmd == "load") {  // or set_config?
    send2server(cmd, name, zk);
  }

  return 0;
} catch (const jubatus::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}

int do_request(
    const string& cmd,
    const string& name,
    const string& ip_port,
    unsigned int n,
    jubatus::server::framework::server_argv argv) {
  string ip;
  int port;
  jubatus::server::common::revert(ip_port, ip, port);

  msgpack::rpc::client c(ip, port);
  c.set_timeout(10);
  cout << "sending " << cmd << " / " << name << " to "
      << ip_port << "..." << std::flush;

  int r;

  if (cmd == "start") {
    r = c.call(cmd, name, n, argv).get<int>();
  } else {
    r = c.call(cmd, name, n).get<int>();
  }

  if (r != 0) {
    cout << "failed." << endl;
    LOG(ERROR) << "can't do '" << cmd << " " << name << "' in " << ip << " "
        << port << ":" << r;
  } else {
    cout << "ok." << endl;
  }
  return r;
}

void send2supervisor(
    const string& cmd,
    const string& type,
    const string& name,
    const string& zkhosts,
    const cmdline::parser& argv) {
  pfi::lang::shared_ptr<jubatus::server::common::lock_service> ls_(
      jubatus::server::common::create_lock_service(
          "zk", zkhosts, 10, "/dev/null"));

  vector<string> list;
  jubatus::server::framework::server_argv server_option;

  if (cmd == "start") {
    string path0 = name.substr(name.find_first_of("/") + 1);
    string path;
    jubatus::server::common::build_actor_path(
        path, type, path0.substr(0, path0.find_first_of("/")));
    ls_->create(path);
    path += "/nodes";
    ls_->create(path);

    server_option.bind_if = argv.get<std::string>("listen_if");
    server_option.threadnum = argv.get<int>("thread");
    server_option.timeout = argv.get<int>("timeout");
    server_option.program_name = type;
    server_option.z = zkhosts;
    server_option.name = name;
    server_option.datadir = argv.get<std::string>("datadir");
    server_option.logdir = argv.get<std::string>("logdir");
    server_option.loglevel = argv.get<int>("loglevel");
    server_option.join = argv.exist("join");

    server_option.interval_sec = argv.get<int>("interval_sec");
    server_option.interval_count = argv.get<int>("interval_count");
  }

  ls_->list(jubatus::server::common::JUBAVISOR_BASE_PATH, list);

  if (list.empty()) {
    LOG(INFO) << "no server to " << cmd << " " << name;
    exit(-1);
  }

  unsigned int N = argv.get<unsigned int>("num");
  if (N == 0) {
    N = list.size();
  }
  vector<string>::const_iterator it;
  unsigned int n = N / list.size();
  for (it = list.begin(); it != list.end(); ++it) {
    do_request(cmd, name, *it, n, server_option);
  }
  n = N % list.size();
  for (size_t i = 0; i < n; ++i) {
    do_request(cmd, name, *it, n, server_option);
  }
}

void send2server(
    const string& cmd,
    const string& name,
    const string& zkhosts) {
  pfi::lang::shared_ptr<jubatus::server::common::lock_service> ls_(
      jubatus::server::common::create_lock_service(
          "zk", zkhosts, 10, "/dev/null"));
  vector<string> list;
  ls_->list(jubatus::server::common::ACTOR_BASE_PATH, list);

  if (list.empty()) {
    LOG(INFO) << "no server to " << cmd << " " << name;
  }

  vector<string>::const_iterator it;
  for (it = list.begin(); it != list.end(); ++it) {
    string ip;
    int port;
    jubatus::server::common::revert(*it, ip, port);

    msgpack::rpc::client c(ip, port);
    c.set_timeout(10);

    cout << "sending " << cmd << " / " << name << " to "
        << *it << "..." << std::flush;

    int r = c.call(cmd, name).get<int>();
    if (r != 0) {
      cout << "failed." << endl;
      LOG(ERROR) << "can't do '" << cmd << " " << name << "' in "
          << ip << " " << port;
    } else {
      cout << "ok." << endl;
    }
  }
}

void show(
    jubatus::server::common::lock_service& z,
    const string& path,
    const string& name) {
  vector<string> list;
  z.list(path, list);
  cout << "\033[34mactive " << name << " members:\033[0m" << endl;
  for (size_t i = 0; i < list.size(); ++i) {
    cout << list[i] << endl;
  }
}

void status(const string& type, const string& name, const string& zkhosts) {
  pfi::lang::shared_ptr<jubatus::server::common::lock_service> ls_(
      jubatus::server::common::create_lock_service(
          "zk", zkhosts, 10, "/dev/null"));
  show(
      *ls_,
      jubatus::server::common::JUBAKEEPER_BASE_PATH + "/" + type,
      "jubakeeper");
  show(*ls_, jubatus::server::common::JUBAVISOR_BASE_PATH, "jubavisor");
  std::string path;
  jubatus::server::common::build_actor_path(path, type, name);
  show(*ls_, path + "/nodes", name);
}
