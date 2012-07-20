// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "rpc_client.hpp"
#include <utility>
#include <glog/logging.h>


using std::string;
using std::vector;
using pfi::lang::shared_ptr;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {

rpc_mclient::rpc_mclient(const vector<std::pair<string, uint16_t> >& hosts,
                         int timeout_sec):
  hosts_(hosts),
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  evbase_(::event_base_new())
{
  if (!evbase_)
    throw std::bad_alloc();

  connect_async_();
  create_fd_event_();
}

rpc_mclient::rpc_mclient(const vector<std::pair<string, int> >& hosts,
                         int timeout_sec):
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  evbase_(::event_base_new())
{
  if (!evbase_)
    throw std::bad_alloc();

  for(size_t i=0; i<hosts.size(); ++i){
    hosts_.push_back(hosts[i]);
  }
  connect_async_();
  create_fd_event_();
}

rpc_mclient::~rpc_mclient()
{
  ::event_base_free(evbase_);
}

void rpc_mclient::connect_async_()
{
  clients_.clear();
  vector<std::pair<string, uint16_t> >::const_iterator it = hosts_.begin(), end = hosts_.end();
  for (; it != end; ++it) {
    shared_ptr<async_client> client(new async_client(it->first, it->second));
    client->connect_async();

    clients_.push_back(client);
  }
}

void rpc_mclient::create_fd_event_()
{
  for (client_list_t::iterator it = clients_.begin(), end = clients_.end();
    it != end; ++it) {
    (*it)->prepare_event(evbase_);
  }
}

void rpc_mclient::register_fd_readable_()
{
  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;

  for (client_list_t::iterator it = clients_.begin(), end = clients_.end();
    it != end; ++it) {
    if ((*it)->is_closed())
      continue;
    (*it)->register_read(timeout);
  }
}

void rpc_mclient::register_fd_writable_(const msgpack::sbuffer& sbuf)
{
  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;

  for (client_list_t::iterator it = clients_.begin(), end = clients_.end();
    it != end; ++it) {
    if ((*it)->is_closed())
      continue;
    (*it)->set_send_buffer(sbuf);
    (*it)->register_write(timeout);
  }
}

void rpc_mclient::send_all(const msgpack::sbuffer& buf)
{
  register_fd_writable_(buf);
  event_base_dispatch(evbase_);
}

} // mprpc
} // common
} // jubatus
