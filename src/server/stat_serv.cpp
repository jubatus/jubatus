// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "stat_serv.hpp"

#include "../common/shared_ptr.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using namespace std;
using namespace jubatus::common;
using namespace jubatus::framework;

namespace jubatus {
namespace server {

stat_serv::stat_serv(const server_argv& a,
                     const cshared_ptr<lock_service>& zk)
    : server_base(a) {
  config_.window_size = 1024; // default till users call set_config
  common::cshared_ptr<stat::mixable_stat> model(new stat::mixable_stat(config_.window_size));
  stat_.set_model(model);

  mixer_.reset(mixer::create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder(
        pfi::lang::shared_ptr<model_bundler>(model_bundler::create(stat_))));

  mixer_->set_mixable_holder(mixable_holder_);
}

stat_serv::~stat_serv() {
}

mixer::mixer* stat_serv::get_mixer() const {
  return mixer_.get();
}

pfi::lang::shared_ptr<mixable_holder> stat_serv::get_mixable_holder() const {
  return mixable_holder_;
}

void stat_serv::get_status(status_t& status) const {
  status.insert(make_pair("storage", stat_.get_model()->type()));
}

bool stat_serv::set_config(const config_data& config) {
  LOG(INFO) << __func__;
  config_ = config;
  common::cshared_ptr<stat::mixable_stat> model(new stat::mixable_stat(config_.window_size));
  stat_.set_model(model);
  return 0;
}

config_data stat_serv::get_config() const {
  return config_;
}

int stat_serv::push(const std::string& key, double value) {
  stat_.get_model()->push(key,value);
  return 0;
}

double stat_serv::sum(const std::string& key) const {
  return stat_.get_model()->sum(key);
}

double stat_serv::stddev(const std::string& key) const {
  return stat_.get_model()->stddev(key);
}

double stat_serv::max(const std::string& key) const {
  return stat_.get_model()->max(key);
}

double stat_serv::min(const std::string& key) const {
  return stat_.get_model()->min(key);
}

double stat_serv::entropy(const std::string& key) const {
#ifdef HAVE_ZOOKEEPER_H
  //FIXME: currently gets old value of entropy when mix completed
  return stat_.get_model()->mixed_entropy();
#else
  return stat_.get_model()->entropy();
#endif
}

double stat_serv::moment(const std::string& key, int n,double c) const {
  return stat_.get_model()->moment(key, n, c);
}

} // namespace server
} // namespace jubatus
