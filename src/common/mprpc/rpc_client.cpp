// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

rpc_result_object rpc_mclient::wait(const std::string& method)
{
  rpc_result_object result;

  if (clients_.empty())
    throw JUBATUS_EXCEPTION(rpc_no_client() << error_method(method));

  register_fd_readable_();
  event_base_dispatch(evbase_);

  size_t count = 0;

  for (client_list_t::iterator it = clients_.begin(), end = clients_.end();
      it != end; ++it) {
    shared_ptr<async_client> client = *it;
    async_client::response_list_t& response_list = client->response();

    try {
      if (client->read_exception())
        client->read_exception()->throw_exception();
      if (client->write_exception())
        client->write_exception()->throw_exception();
      if (response_list.empty())
        throw JUBATUS_EXCEPTION(rpc_no_result() << error_method(method));

      // If implement RPC specification strictly, you must find response by msgid,
      // but pfi::network::mprpc::rpc_server does not support RPC pipelining.
      rpc_response_t res = response_list.front();
      response_list.erase(response_list.begin());
      if (res.has_error()) {
        if (res.error().type == msgpack::type::POSITIVE_INTEGER) {
          // error code defined in pficommon/mprpc/message.h
          switch (static_cast<unsigned int>(res.error().via.u64)) {
          case pfi::network::mprpc::METHOD_NOT_FOUND:
            throw JUBATUS_EXCEPTION(rpc_method_not_found() << error_method(method));

          case pfi::network::mprpc::TYPE_MISMATCH:
            throw JUBATUS_EXCEPTION(rpc_type_error() << error_method(method));

          default:
            throw JUBATUS_EXCEPTION(rpc_call_error()
                << error_method(method)
                << jubatus::exception::error_message(std::string("rpc_server error: " + pfi::lang::lexical_cast<std::string>(res.error().via.u64))));
          }
        } else {
          // MEMO: other error object returned
          throw JUBATUS_EXCEPTION(rpc_call_error()
              << error_method(method)
              << jubatus::exception::error_message("error response: " + pfi::lang::lexical_cast<std::string>(res.error())));
        }
      }

      result.response.push_back(res);

      count++;

    // continue process next result when exception thrown
    } catch (...) {
      // store exception_thrower to list of error
      result.error.push_back(rpc_error(client->host(), client->port(), jubatus::exception::get_current_exception()));

      // clear last exception set by libevent callback
      client->context_->read_exception.reset();
      client->context_->write_exception.reset();
    }
  }

  if (!count) {
    rpc_no_result e;
    if (result.has_error())
      e << error_multi_rpc(result.error);
    throw JUBATUS_EXCEPTION(e << error_method(method));
  }

  return result;
}

} // mprpc
} // common
} // jubatus
