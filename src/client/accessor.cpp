// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "../server/classifier_rpc.hpp"
#include "../jubakeeper/rpc.hpp"
#include "accessor.hpp"

#include <pficommon/network/mprpc.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>

#include <vector>
#include <exception>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace jubatus {
namespace client {

void parse_hostnames(const string& hosts, vector<connection_info>& out)
{
  out.clear();
  string tmp(hosts);

  vector<string> hs = pfi::data::string::split(hosts, string(","));
  for (size_t i = 0; i < hs.size(); ++i) {
    vector<string> ps = pfi::data::string::split(hs[i], string(":"));
    out.push_back(make_pair(ps[0], pfi::lang::lexical_cast<int>(ps[1])));
  }
}


accessor::accessor(const string& hosts, double timeout)
    : timeout_(timeout) {
  parse_hostnames(hosts, servers_);
  if (servers_.empty())
    throw runtime_error("no servers to connect.");
};

accessor::~accessor()
{
}

void accessor::find_one(connection_info &ret)
{
  ret = servers_[rng_(servers_.size())];
}

void accessor::find_two(const string& key, vector<connection_info>& out){
  out = servers_;
}

}
}

