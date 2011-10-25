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
#include <map>
#include <vector>

#include <pficommon/math/random.h>
#include <pficommon/lang/scoped_ptr.h>

#include "../common/exception.hpp"
#include "../server/classifier_rpc.hpp"

namespace jubatus {
namespace client {

class classifier_impl;

class classifier {
public:
  classifier(const std::string &hosts,
             const std::string &name,
             double timeout);
  ~classifier();

  void set_config(const jubatus::classifier::config_data& config);
  jubatus::classifier::config_data get_config();

  void train(const std::vector<std::pair<std::string, datum> >& data);
  std::vector<jubatus::classifier::estimate_results> classify(const std::vector<datum>& data);

  std::map<connection_info, std::map<std::string,std::string> > get_status();
  void save(const std::string& type, const std::string& id);
  void load(const std::string& type, const std::string& id);

private:
  pfi::lang::scoped_ptr<classifier_impl> impl_;
};

} //namespace client
} //namespace jubatus
