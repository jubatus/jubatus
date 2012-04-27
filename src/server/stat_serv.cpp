// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "stat_serv.hpp"

namespace jubatus {
namespace server {

stat_serv::stat_serv(const framework::server_argv& a)
  :jubatus_serv(a)
{
  config_.window_size = 1024; // default till users call set_config
  stat_ =  pfi::lang::shared_ptr<stat::stat>(new stat::stat(config_.window_size)); //FIXME
}

stat_serv::~stat_serv() {
}

// after load(..) called, users reset their own data
void stat_serv::after_load(){
  //  stat_.reset();
};

bool stat_serv::set_config(const config_data& config){
  config_ = config;
  stat_ =  pfi::lang::shared_ptr<stat::stat>(new stat::stat(config_.window_size)); //FIXME
  return 0;
}
config_data stat_serv::get_config()const{
  return config_;
}
int stat_serv::push(const std::string& key, double value){
  stat_->push(key,value);
  return 0;
}
double stat_serv::sum(const std::string& key) const {
  return stat_->sum(key);
}
double stat_serv::stddev(const std::string& key) const {
  return stat_->stddev(key);
}
double stat_serv::max(const std::string& key) const {
  return stat_->max(key);
}
double stat_serv::min(const std::string& key) const {
  return stat_->min(key);
}
double stat_serv::entropy(const std::string& key) const {
  return stat_->entropy();
}
double stat_serv::moment(const std::string& key, int n,double c) const{
  return stat_->moment(key, n, c);
}


} // namespace server
} // namespace jubatus
