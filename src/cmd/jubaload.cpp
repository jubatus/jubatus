// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <pficommon/text/json.h>
#include "commonrpc_client.hpp"
#include "../common/exception.hpp"
#include "../common/cmdline.h"

static const std::string PROGNAME(JUBATUS_APPNAME "load");
static const std::string VERSION(JUBATUS_VERSION);

using namespace std;
using namespace pfi::text::json;
using namespace jubatus::client;

bool callload(const string&, const int, const string&, const string&);

int main(int args, char** argv) try {
  cmdline::parser p;
  p.add<string>("server", 's', "server or keeper ip:port", false, "127.0.0.1:9199");
  p.add<string>("name", 'n', "learning machine name", false, "default");
  p.add<string>("model", 'm', "model name", false, "default");
  p.parse_check(args, argv);

  string snpn = p.get<string>("server");
  string name = p.get<string>("name");
  string model = p.get<string>("model");

  string server;
  int port;

  if (iprevert(snpn, server, port)){
    if(callload(server, port, name, model)){
    std::cout << "load " << model << " successfully" << std::endl;
   }
  }
  return 0;
} catch (const jubatus::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}

bool callload(const string& server,const int port, const string& name, const string& model){
    
    commonrpc crpc(server, port, 10);
    return crpc.load(name, model);
}



