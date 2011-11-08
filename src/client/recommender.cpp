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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 
#include "../server/recommender_rpc.hpp"

#include "recommender.hpp"
#include "accessor.hpp"
#include <pficommon/network/mprpc.h>

#include <iostream>
#include <string>
#include <map>
#include <cassert>

#include "../common/rpc_util.hpp"
#include "../jubakeeper/rpc.hpp"

using namespace std;
using namespace jubatus::recommender;

namespace jubatus {
namespace client {

class recommender_impl {
public:
  recommender_impl(const std::string& hosts,
                   const std::string& name,
                   double timeout)
    : acc_(hosts, timeout),
      name_(name),
      timeout_(timeout) {}
  
  ~recommender_impl() {}

  void set_config(const jubatus::recommender::config_data& config) {
    check_throw(make_client().call_set_config(name_, config));
  }

  jubatus::recommender::config_data get_config() {
    return return_or_throw(make_client().call_get_config(name_));
  }

  void clear_row(const vector<string>& ids)
  {
    // for (vector<connection_info>::const_iterator p = acc_.servers().begin();
    //      p != acc_.servers().end(); ++p)
    //   mprpc_client(p->first, p->second, timeout_).call_clear_row(ids);
  }


  void update_row(const rows& dat)
  {
    if (dat.empty()) return;
    
    // TODO: group dat by nodes
    for (size_t i = 0; i < dat.size(); ++i) {
      vector<connection_info> nodes;
      acc_.find_two(dat[i].first, nodes);
      if(nodes.empty())
        throw runtime_error("cannot find cht for " + dat[i].first);
      
      rows ds(1, dat[i]);
      // for(size_t j = 0; j < nodes.size(); ++j)
      //   mprpc_client(nodes[j].first, nodes[j].second, timeout_).call_update_row(ds);
    }
  }

  void build()
  {
    // only standalone mode.
    
    connection_info conn;
    acc_.find_one(conn);
    // mprpc_client(conn.first, conn.second, timeout_).call_build();
  }

  datum complete_row_from_id(const string& id)
{
  std::vector<connection_info> nodes;
  acc_.find_two(id, nodes);
  if(nodes.empty()) throw runtime_error("no nodes to do RPC");
  // size_t ix = rng_(nodes.size());
  // return mprpc_client(nodes[ix].first, nodes[ix].second, timeout_).call_complete_row_from_id(id);
  return datum();
}

datum complete_row_from_data(const datum& dat)
{
  connection_info conn;
  acc_.find_one(conn);
  //  return mprpc_client(conn.first, conn.second, timeout_).call_complete_row_from_data(dat);
  return datum();
}

similar_result similar_row_from_id(const string& id, size_t ret_num)
{
  std::vector<connection_info> nodes;
  acc_.find_two(id, nodes);
  if(nodes.empty()) throw runtime_error("no nodes to do RPC");
  //  size_t ix = rng_(nodes.size());
  //  return mprpc_client(nodes[ix].first, nodes[ix].second, timeout_).call_similar_row_from_id(id, ret_num);
  return similar_result();
}

similar_result similar_row_from_data(const datum& dat, size_t ret_num)
{
  connection_info conn;
  acc_.find_one(conn);
  //  return mprpc_client(conn.first, conn.second, timeout_).call_similar_row_from_data(dat, ret_num);
  return similar_result();
}

datum decode_row(const string& id)
{
  std::vector<connection_info> nodes;
  acc_.find_two(id, nodes);
  if(nodes.empty()) throw runtime_error("no nodes to do RPC");
  //  size_t ix = rng_(nodes.size());
  //  return mprpc_client(nodes[ix].first, nodes[ix].second, timeout_).call_decode_row(id);
  return datum();
}

rows get_all_rows()
{
  connection_info conn;
  acc_.find_one(conn);
  //  return mprpc_client(conn.first, conn.second, timeout_).call_get_all_rows();
  return rows();
}

void save(const string& type, const string& id)
{
  /*
  for (vector<connection_info>::const_iterator p = acc_.servers().begin();
       p != acc_.servers().end(); ++p)
    mprpc_client(p->first, p->second, timeout_).call_save(type, id);
  */
  assert(!"recommender::save() is not implemented");
}

void load(const string& type, const string& id)
{
  /*
  for (vector<connection_info>::const_iterator p = acc_.servers().begin();
       p != acc_.servers().end(); ++p)
    mprpc_client(p->first, p->second, timeout_).call_load(type, id);
  */
  assert(!"recommender::load() is not implemented");
}

  result<map<connection_info, map<string, string> > >
  get_status_fun(const connection_info& con) {
    return result<map<connection_info, map<string, string> > >();
  }

  map<connection_info, map<string,string> > get_status() {
    return return_or_throw(make_client().call_get_status(name_));
  }

  // FIXME
  // void save(const string& type, const string& id) {
  //   check_throw(make_client().call_save(name_, type, id));
  // }

  // void load(const string& type, const string& id) {
  //   check_throw(make_client().call_save(name_, type, id));
  // }

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

recommender::recommender(const std::string& hosts, const std::string& name,
                         double timeout, bool use_jubakeepers)
  : impl_(new recommender_impl(hosts, name, timeout)) {
}

recommender::~recommender() {
}

void recommender::set_config(const config_data &config)
{
  impl_->set_config(config);
}

config_data recommender::get_config()
{
  return impl_->get_config();
}

void recommender::clear_row(const vector<string>& ids)
{
  impl_->clear_row(ids);
}

void recommender::update_row(const rows& dat)
{
  impl_->update_row(dat);
}

void recommender::build()
{
  impl_->build();
}

datum recommender::complete_row_from_id(const string& id)
{
  return impl_->complete_row_from_id(id);
}

datum recommender::complete_row_from_data(const datum& dat)
{
  return impl_->complete_row_from_data(dat);
}

similar_result recommender::similar_row_from_id(const string& id, size_t ret_num)
{
  return impl_->similar_row_from_id(id,ret_num);
}

similar_result recommender::similar_row_from_data(const datum& dat, size_t ret_num)
{
  return impl_->similar_row_from_data(dat,ret_num);
}

datum recommender::decode_row(const string& id)
{
  return impl_->decode_row(id);
}

rows recommender::get_all_rows()
{
  return impl_->get_all_rows();
}

void recommender::save(const string& type, const string& id)
{
  impl_->save(type,id);
}

void recommender::load(const string& type, const string& id)
{
  impl_->load(type,id);
}

} // namespace clinent
} // namespace jubatus
