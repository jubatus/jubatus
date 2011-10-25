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

#pragma once

#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#include <pficommon/lang/shared_ptr.h>

#include "zk.hpp"

namespace jubatus{

  // using SHA-512. see crypt(2). recommended: glibc >= 2.7
  static const std::string SALT_BASE = "$6$jubatus$";

  static const unsigned int NUM_VSERV = 8;
  
  // this function does not seem pure, take care when calling from multiple threads
  std::string make_hash(const std::string& key);

  class cht{
  public:
    cht(zk&, const std::string&);
    ~cht();

    // node :: ip_port
    // register_node :: node -> bool;
    bool register_node(const std::string&, int);

    // find(hash)    :: key -> [node] where  hash(node0) <= hash(key) < hash(node1) < hash(node2) < ...
    bool find(const std::string& host, int port, std::vector<std::pair<std::string,int> >&);
    bool find(const std::string&, std::vector<std::pair<std::string,int> >&);

  private:
    std::string name_;
    pfi::lang::shared_ptr<zk> zk_;
  }; //cht  
} //jubatu
