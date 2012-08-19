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

#pragma once

#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#include "shared_ptr.hpp"
#include "lock_service.hpp"

#include <pficommon/lang/cast.h>

namespace jubatus{
namespace common{

  static const unsigned int NUM_VSERV = 8;
  
  // this function does not seem pure, take care when calling from multiple threads
  std::string make_hash(const std::string& key);

  class cht{
  public:
    cht(cshared_ptr<lock_service>, const std::string& type, const std::string& name);
    ~cht();

    // node :: ip_port
    // register_node :: node -> bool;
    void register_node(const std::string&, int);
    
    template <typename T>
    bool find(const T& t, std::vector<std::pair<std::string,int> > & ret, size_t s){
      std::string k = pfi::lang::lexical_cast<std::string>(t);
      return find(k, ret, s);
    };

    // find(hash)    :: key -> [node] where  hash(node0) <= hash(key) < hash(node1) < hash(node2) < ...
    bool find(const std::string& host, int port, std::vector<std::pair<std::string,int> >&, size_t);
    bool find(const std::string&, std::vector<std::pair<std::string,int> >&, size_t);

    std::pair<std::string,int> find_predecessor(const std::string& host, int port);
    std::pair<std::string,int> find_predecessor(const std::string&);

    // run just once in starting up the process: creates <name>/cht directory.
    static void setup_cht_dir(lock_service&, const std::string&, const std::string&);

  private:

    bool get_hashlist_(const std::string& key, std::vector<std::string>&);

    const std::string type_;
    const std::string name_;
    cshared_ptr<lock_service> lock_service_;
  }; //cht  
}
}
