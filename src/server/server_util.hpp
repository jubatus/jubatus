#pragma once

#include <string>
#include <sstream>
#include "../common/cmdline.h"
#include "../common/util.hpp"

static const std::string VERSION(JUBATUS_VERSION);

#define SET_PROGNAME(s) \
  static const std::string PROGNAME(JUBATUS_APPNAME "_" s);

namespace jubatus {

struct server_argv {
  server_argv(int args, char** argv){
    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "thread number", false, 2);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
    p.add<std::string>("name", 'n', "learning machine instance name", true);
    p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");
    p.parse_check(args, argv);

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
    z = p.get<std::string>("zookeeper");
    name = p.get<std::string>("name");
    tmpdir = p.get<std::string>("tmpdir");
    eth = jubatus::util::get_ip("eth0");
  };
  int port;
  int timeout;
  int threadnum;
  std::string z;
  std::string name;
  std::string tmpdir;
  std::string eth;

  std::string boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };
};


};
