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

#include "../stat/mixable_stat.hpp"
#include "jubatus_serv.hpp"
#include "stat_types.hpp"

namespace jubatus{
namespace server{

struct mixable_stat : public framework::mixable<jubatus::stat::mixable_stat,
						std::pair<double,size_t>,
						mixable_stat>
{
public:
  void clear(){}
  mixable_stat()
  {
    set_default_mixer();
  }
  static std::pair<double,size_t> get_diff(const jubatus::stat::mixable_stat* model)
  {
    return model->get_diff();
  }
  static int reduce(const jubatus::stat::mixable_stat*,
		    const std::pair<double,size_t>& v, std::pair<double,size_t>& acc)
  {
    jubatus::stat::mixable_stat::reduce(v, acc);
    return 0;
  }

  static int put_diff(jubatus::stat::mixable_stat* model,
		      const std::pair<double,size_t>& v)
  {
    model->put_diff(v);
    return 0;
  }

};

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

  std::map<std::string,std::map<std::string,std::string> > get_status()const;

private:
  jubatus::config_data config_;
  server::mixable_stat stat_;
};

}
} // namespace jubatus
