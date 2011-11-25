#include "server_util.hpp"
#include "../common/cmdline.h"

namespace jubatus {
  
  server_argv::server_argv(int args, char** argv){
    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 2);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
    p.add<std::string>("name", 'n', "learning machine instance name", true);
    p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");
    p.add("join",    'j', "join to the existing cluster");

    p.add<int>("interval_sec", 's', "mix interval by seconds", 5);
    p.add<int>("interval_count", 'i', "mix interval by update count", 1024);

    p.parse_check(args, argv);

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
    z = p.get<std::string>("zookeeper");
    name = p.get<std::string>("name");
    tmpdir = p.get<std::string>("tmpdir");
    eth = jubatus::util::get_ip("eth0");
    join = p.get<bool>("join");

    interval_sec = p.get<int>("interval_sec");
    interval_count = p.get<int>("interval_count");
  };

  server_argv::server_argv():
    join(false), port(9199), timeout(10), threadnum(2), z(""), name(""),
    tmpdir("/tmp"), eth(""), interval_sec(5), interval_count(1024)
  {
  };

  keeper_argv::keeper_argv(int args, char** argv){
    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 16);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false, "localhost:2181");

    p.parse_check(args, argv);

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
    z = p.get<std::string>("zookeeper");
    eth = jubatus::util::get_ip("eth0");
  };

  keeper_argv::keeper_argv():
    port(9199), timeout(10), threadnum(16), z("localhost:2181"), eth("")
  {
  };
}
