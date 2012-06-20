// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "../stat/stat.hpp"
#include "jubatus_serv.hpp"
#include "stat_types.hpp"

namespace jubatus{
namespace server{

// class mixable_stat : public frameworK::mixable<>
// {
// };

class stat_serv : public framework::jubatus_serv
{
public:
  stat_serv(const framework::server_argv&);
  virtual ~stat_serv();

  void after_load();

  bool set_config(const config_data&);
  config_data get_config()const;
  int push(const std::string& key, double value);
  double sum(const std::string&) const ;
  double stddev(const std::string&) const ;
  double max(const std::string&) const ;
  double min(const std::string&) const ;
  double entropy(const std::string&) const ;
  double moment(const std::string&, int, double) const;

private:
  jubatus::config_data config_;
  pfi::lang::shared_ptr<jubatus::stat::stat> stat_;
};

}
} // namespace jubatus
