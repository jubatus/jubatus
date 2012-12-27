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
#include <glog/logging.h>

#include <iostream>
#include <iomanip>
#include <pficommon/text/json.h>

#include "../common/util.hpp"
#include "../common/network.hpp"
#include "../common/cmdline.h"
#include "../common/exception.hpp"
#include "../common/membership.hpp"
#include "../common/config.hpp"

namespace jubatus {
namespace framework {

static const std::string VERSION(JUBATUS_VERSION);

void print_version(const std::string& progname){
  std::cout << "jubatus-" << VERSION << " (" << progname << ")" << std::endl;
}

std::string get_conf(const server_argv& a){

  config_json conf;

  try{
    if (a.is_standalone()) {
      conf.load_json(a.configpath);
    }else{
#ifdef HAVE_ZOOKEEPER_H
      conf.load_json(a.z, a.type, a.name);
#endif
    }
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(ERROR) << e.what();
    exit(1);
  }
  return conf.config;
}

void config_json::load_json(const std::string& zkhosts, const std::string& type, const std::string& name)
{
#ifdef HAVE_ZOOKEEPER_H
  jubatus::common::config_fromzk(*ls, type, name, config);
#endif
}

void config_json::load_json(const std::string& filepath)
{
    jubatus::common::config_fromlocal(filepath, config);
}

server_argv::server_argv(int args, char** argv, const std::string& type)
  : type(type)
{
  google::InitGoogleLogging(argv[0]);

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<int>("thread", 'c', "concurrency = thread number", false, 2);
  p.add<int>("timeout", 't', "time out (sec)", false, 10);
  p.add<std::string>("tmpdir", 'd', "directory to save and load models", false, "/tmp");
  p.add<std::string>("logdir", 'l', "directory to output logs (instead of stderr)", false);
  p.add<std::string>("configpath", 'f', "config option need to specify json file when standalone mode (without ZK mode)", false, "");

#ifdef HAVE_ZOOKEEPER_H
  p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
  p.add<std::string>("name", 'n', "learning machine instance name", false);
  p.add("join", 'j', "join to the existing cluster");
  p.add<int>("interval_sec", 's', "mix interval by seconds", false, 16);
  p.add<int>("interval_count", 'i', "mix interval by update count", false, 512);
#endif

  // APPLY CHANGES TO JUBAVISOR WHEN ARGUMENTS MODIFIED

  p.add("version", 'v', "version");

  p.parse_check(args, argv);

  if( p.exist("version") ){
    print_version(jubatus::util::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  program_name = jubatus::util::get_program_name();
  tmpdir = p.get<std::string>("tmpdir");
  logdir = p.get<std::string>("logdir");
  configpath = p.get<std::string>("configpath");
  
  //    eth = "localhost";
  eth = jubatus::common::get_default_v4_address();

#ifdef HAVE_ZOOKEEPER_H
  z = p.get<std::string>("zookeeper");
  name = p.get<std::string>("name");
  join = p.exist("join");
  interval_sec = p.get<int>("interval_sec");
  interval_count = p.get<int>("interval_count");
#else
  z = "";
  name = "";
  join = false;
  interval_sec = 16;
  interval_count = 512;
#endif

  if(!is_standalone() && name.empty()){
    std::cerr << "can't start multinode mode without name specified" << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if (is_standalone() && configpath.empty()){
    std::cerr << "can't start standalone mode without configpath specified" << std::endl;
    std::cerr << p.usage() << std::endl;
    exit(1);
  }

  if(p.exist("logdir")){
    set_log_destination(jubatus::util::get_program_name());
  } else {
    google::LogToStderr();
  }

  boot_message(jubatus::util::get_program_name());
}

server_argv::server_argv():
  join(false), port(9199), timeout(10), threadnum(2), z(""), name(""),
  tmpdir("/tmp"), logdir(""), eth("localhost"), interval_sec(5), interval_count(1024)
{
}

void server_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth << ":" << port << '\n';
  ss << "    pid            : " << getpid() << '\n';
  ss << "    user           : " << getenv("USER") << '\n';
  ss << "    mode           : ";
  if(is_standalone()) {
    ss << "standalone mode\n";
  } else {
    ss << "multinode mode\n";
  }
  ss << "    timeout        : " << timeout << '\n';
  ss << "    thread         : " << threadnum << '\n';
  ss << "    tmpdir         : " << tmpdir << '\n';
  ss << "    logdir         : " << logdir << '\n';
#ifdef HAVE_ZOOKEEPER_H
  ss << "    zookeeper      : " << z << '\n';
  ss << "    name           : " << name << '\n';
  ss << "    join           : " << std::boolalpha << join << '\n';
  ss << "    interval sec   : " << interval_sec << '\n';
  ss << "    interval count : " << interval_count << '\n';
#endif
  LOG(INFO) << ss.str();
}

void server_argv::set_log_destination(const std::string& progname) const {
  std::ostringstream basename;
  basename <<  progname << '.' << eth << '_' << port;
  for(int severity = 0; severity < google::NUM_SEVERITIES; severity++) {
    std::string log = logdir + '/';
    log += basename.str() + '.';
    if (!name.empty()) {
      log += name + '.';
    }
    log += google::GetLogSeverityName(severity);
    log += ".log.";
    std::string link = basename.str() + '.';
    link += pfi::lang::lexical_cast<std::string>(getpid());
    google::SetLogDestination(severity, log.c_str());
    google::SetLogSymlink(severity, link.c_str());
  }
}

std::string get_server_identifier(const server_argv& a) {
  std::stringstream ss;
  ss << a.eth;
  ss << "_";
  ss << a.port;
  return ss.str();
}

keeper_argv::keeper_argv(int args, char** argv, const std::string& t)
  : type(t)
{
  google::InitGoogleLogging(argv[0]);

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<int>("thread", 'c', "concurrency = thread number", false, 16);
  p.add<int>("timeout", 't', "time out (sec)", false, 10);

  p.add<std::string>("zookeeper", 'z', "zookeeper location", false, "localhost:2181");
  p.add<std::string>("logdir", 'l', "directory to output logs (instead of stderr)", false);
  p.add("version", 'v', "version");

  p.parse_check(args, argv);

  if( p.exist("version") ){
    print_version(jubatus::util::get_program_name());
    exit(0);
  }

  port = p.get<int>("rpc-port");
  threadnum = p.get<int>("thread");
  timeout = p.get<int>("timeout");
  program_name = jubatus::util::get_program_name();
  z = p.get<std::string>("zookeeper");
  logdir = p.get<std::string>("logdir");
  eth = jubatus::common::get_default_v4_address();

  if(p.exist("logdir")){
    set_log_destination(jubatus::util::get_program_name());
  } else {
    google::LogToStderr();
  }

  boot_message(jubatus::util::get_program_name());
}

keeper_argv::keeper_argv():
  port(9199), timeout(10), threadnum(16), z("localhost:2181"), logdir(""), eth("")
{
}

void keeper_argv::boot_message(const std::string& progname) const {
  std::stringstream ss;
  ss << "starting " << progname << " " << VERSION << " RPC server at " << eth << ":" << port << '\n';
  ss << "    pid            : " << getpid() << '\n';
  ss << "    user           : " << getenv("USER") << '\n';
  ss << "    timeout        : " << timeout << '\n';
  ss << "    thread         : " << threadnum << '\n';
  ss << "    logdir         : " << logdir << '\n';
  ss << "    zookeeper      : " << z << '\n';
  LOG(INFO) << ss.str();
}

void keeper_argv::set_log_destination(const std::string& progname) const {
  std::ostringstream basename;
  basename <<  progname << '.' << eth << '_' << port;
  for(int severity = 0; severity < google::NUM_SEVERITIES; severity++) {
    std::string log = logdir + '/';
    log += basename.str() + '.';
    log += google::GetLogSeverityName(severity);
    log += ".log.";
    std::string link = basename.str() + '.';
    link += pfi::lang::lexical_cast<std::string>(getpid());
    google::SetLogDestination(severity, log.c_str());
    google::SetLogSymlink(severity, link.c_str());
  }
}

common::cshared_ptr<jubatus::common::lock_service> ls;

void atexit(){
#ifdef HAVE_ZOOKEEPER_H
  if(ls)
    ls->force_close();
#endif
}

} // framework
} // jubatus
