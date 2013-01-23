// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <sstream>
#include <fstream>
#include <cstring>

#include <pficommon/lang/function.h>

#include "../common/exception.hpp"
#include "../common/zk.hpp"
#include "../common/membership.hpp"
#include "../common/config.hpp"
#include "../common/cmdline.h"

static const std::string PROGNAME(JUBATUS_APPNAME "config");
static const std::string VERSION(JUBATUS_VERSION);

using std::string;
using std::cout;
using std::endl;
using std::vector;

void usage();
void set_config(const string&, const string&, const string&, const string&);
void delete_config(const string&, const string&, const string&);
void get_config(const string&, const string&, const string&);
void get_configs(const string&);
void print_config(jubatus::common::lock_service&, const string&, const string&);
void get_all_config_paths(jubatus::common::lock_service&,
                          std::vector<std::pair<string, string> >&);

int main(int args, char** argv)
try {
  cmdline::parser p;
  p.add<std::string>("cmd", 'c',
                     "command to operate config (write|read|delete|list)",
                     true);
  p.add<std::string>("file", 'f', "[write] config file to set", false, "");
  p.add<std::string>("type", 't', "[write|read|delete] learning machine type",
                     false, "");
  p.add<std::string>("name", 'n', "[write|read|delete] learning machine name",
                     false, "");
  p.add<std::string>(
      "zookeeper", 'z',
      "ZooKeeper location environment: 'ZK' is available instead", false);

  p.add("debug", 'd', "debug mode");
  p.parse_check(args, argv);

  google::InitGoogleLogging(argv[0]);
  if (p.exist("debug")) {
    google::LogToStderr();  // only when debug
  }

  string zk;
  char* zkenv = std::getenv("ZK");

  if (p.get<std::string>("zookeeper") != "") {
    zk = p.get<std::string>("zookeeper");
  } else if (zkenv) {
    zk = zkenv;
  } else {
    cout
        << "can't get ZK location: set 'ZK' environment or specify '-z <somezkaddrs>"
        << endl;
    cout << p.get<std::string>("zookeeper") << endl;
    ;
    cout << p.usage() << endl;
    exit(1);
  }

  string cmd = p.get<std::string>("cmd");
  string type = p.get<std::string>("type");
  string name = p.get<std::string>("name");

  if (cmd == "write" || cmd == "delete" || cmd == "read") {
    if (type == "" || name == "") {
      cout << "can't excute " << cmd << " without type and name specified"
          << endl;
      cout << "type: " << type << " name: " << name << endl;
      cout << p.usage() << endl;
      exit(1);
    }

    if (cmd == "write")
      set_config(zk, type, name, p.get<std::string>("file"));

    if (cmd == "delete")
      delete_config(zk, type, name);

    if (cmd == "read")
      get_config(zk, type, name);
  } else if (cmd == "list") {
    get_configs(zk);
  } else {
    cout << "unknown cmd: " << cmd << endl;
    ;
    cout << p.usage() << endl;
    exit(1);
  }

  return 0;

}
catch (const jubatus::exception::jubatus_exception& e) {
  cout << e.diagnostic_information(true) << endl;
  exit(1);
}

void set_config(const string& zkhosts, const string& type, const string& name,
                const string& configfile) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_(
      jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  jubatus::common::prepare_jubatus(*ls_, type, name);

  string config;
  jubatus::common::config_fromlocal(configfile, config);
  jubatus::common::config_tozk(*ls_, type, name, config);
}

void delete_config(const string& zkhosts, const string& type,
                   const string& name) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_(
      jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  jubatus::common::remove_config_fromzk(*ls_, type, name);
}

void get_config(const string& zkhosts, const string& type, const string& name) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_(
      jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  print_config(*ls_, type, name);
}

void get_configs(const string& zkhosts) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_(
      jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  std::vector < std::pair<string, string> > paths;
  get_all_config_paths(*ls_, paths);

  for (size_t i = 0; i < paths.size(); i++) {
    cout << "type: " << paths[i].first << ", name: " << paths[i].second << endl;
    print_config(*ls_, paths[i].first, paths[i].second);
    cout << '\n';
  }

}

void print_config(jubatus::common::lock_service& z, const string& type,
                  const string& name) {
  string config;
  jubatus::common::config_fromzk(z, type, name, config);
  cout << config;
}

void get_all_config_paths(jubatus::common::lock_service& z,
                          std::vector<std::pair<string, string> >& ret) {
  ret.clear();

  std::vector < std::string > types;
  if (!z.list(jubatus::common::CONFIG_BASE_PATH, types) || types.empty())
    return;

  for (size_t i = 0; i < types.size(); i++) {
    std::vector < std::string > names;
    std::string path;
    path = jubatus::common::CONFIG_BASE_PATH + '/' + types[i];
    if (!z.list(path, names) || names.empty())
      continue;

    for (size_t j = 0; j < names.size(); j++) {
      std::string config_path;
      config_path = path + '/' + names[j];
      ret.push_back(make_pair(types[i], names[j]));
    }
  }
}
