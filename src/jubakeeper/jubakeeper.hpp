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
#include <pficommon/network/mprpc.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/math/random.h>

#include "../common/cht.hpp"
#include "../common/zk.hpp"
#include "../server/classifier_rpc.hpp"
#include <vector>
#include <map>

namespace jubatus{

class jubakeeper{
public:
  jubakeeper(const std::string&, int, int = 10, const std::string& logfile = "");
  ~jubakeeper();
  
  // all node that belong to the name
  result<std::vector<std::pair<std::string, int> > > lookup(std::string);

  // two node that that interposes the key
  // [node0, node1] where hash(node0) <= key < hash(node1) 
  result<std::vector<std::pair<std::string, int> > > lookup_cht(std::string, std::string);

  // should be same interface with classifer_serv
  result<int> set_config(std::string, classifier::config_data);
  result<classifier::config_data> get_config(std::string);
  result<int> train(std::string,std::vector<std::pair<std::string, datum> > data);
  result<std::vector<classifier::estimate_results> > classify(std::string,std::vector<datum> data);
  result<int> save(std::string, std::string, std::string);
  result<int> load(std::string, std::string, std::string);
  result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > get_status(std::string);

private:
  void get_members_(const std::string& name, std::vector<std::pair<std::string, int> >&);

  jubakeeper();
  zk zk_;
  std::string name_;

  pfi::concurrent::mutex m_;
  int max_children_;
  int timeout_;
  pfi::math::random::mtrand rng_;
};

}//jubatus
