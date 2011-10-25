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

#include "classifier.hpp"
#include "accessor.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>

#include <iostream>
#include <string>

#include "../common/rpc_util.hpp"
#include "../jubakeeper/rpc.hpp"

using namespace std;
using namespace jubatus::classifier;
using namespace pfi::lang;

namespace jubatus {
namespace client {

class classifier_impl {
public:
  classifier_impl(const string &hosts,
                  const string& name,
                  double timeout)
      : acc_(hosts, timeout),
        name_(name),
        timeout_(timeout) {}

  ~classifier_impl() {}

  void set_config(const jubatus::classifier::config_data& config) {
    check_throw(make_client().call_set_config(name_, config));
  }

  jubatus::classifier::config_data get_config() {
    return return_or_throw(make_client().call_get_config(name_));
  }

  void train(const vector<pair<string, datum> >& data) {
    check_throw(make_client().call_train(name_, data));
  }

  vector<jubatus::classifier::estimate_results> classify(const vector<datum>& data) {
    return return_or_throw(make_client().call_classify(name_, data));
  }

  result<map<connection_info, map<string, string> > >
  get_status_fun(const connection_info& con) {
    return result<map<connection_info, map<string, string> > >();
  }

  map<connection_info, map<string,string> > get_status() {
    return return_or_throw(make_client().call_get_status(name_));
  }

  void save(const string& type, const string& id) {
    check_throw(make_client().call_save(name_, type, id));
  }

  void load(const string& type, const string& id) {
    check_throw(make_client().call_save(name_, type, id));
  }

private:
  mprpc_client make_client() {
    connection_info conn;
    acc_.find_one(conn);
    return make_client(conn);
  }

  mprpc_client make_client(const connection_info& conn) {
    return mprpc_client(conn.first, conn.second, timeout_);
  }

  accessor acc_;
  string name_;
  double timeout_;
};


classifier::classifier(const string& hosts,
                       const string& name,
                       double timeout)
    : impl_(new classifier_impl(hosts, name, timeout)) {
}

classifier::~classifier() {
}

void classifier::set_config(const config_data& config) {
  impl_->set_config(config);
}

config_data classifier::get_config() {
  return impl_->get_config();
}

void classifier::train(const vector<pair<string, datum> >& data) {
  impl_->train(data);
}

vector<estimate_results> classifier::classify(const vector<datum>& data) {
  return impl_->classify(data);
}

map<connection_info, map<string, string> > classifier::get_status() {
  return impl_->get_status();
}

void classifier::save(const string& type, const string& id) {
  impl_->save(type, id);
}

void classifier::load(const string& type, const string& id) {
  impl_->load(type, id);
}

} // namespace client
} // namespace jubatus
