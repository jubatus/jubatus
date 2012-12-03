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

#include "rpc_server.hpp"

using namespace msgpack::rpc;

namespace jubatus { namespace common { namespace mprpc {

// rpc_server
//   Msgpack-RPC based server with 'hashed' dispatcher. rpc_server can add RPC method on-the-fly.
//
void rpc_server::dispatch( request req ) {
  std::string method;
  req.method().convert(&method);

  func_map::iterator fun = funcs.find( method );
  if ( fun == funcs.end() ) {
    req.error( msgpack::rpc::NO_METHOD_ERROR, method );
    return;
  }

  try {
    fun->second->invoke( req );
  } catch (const msgpack::type_error &e) {
    req.error( msgpack::rpc::ARGUMENT_ERROR, std::string(e.what()) );
  } catch( const std::exception &e ) {
    req.error( std::string( e.what()));
  }
}

void rpc_server::add_inner(const std::string &name, pfi::lang::shared_ptr<invoker_base> invoker) {
  funcs[name] = invoker;
}

bool rpc_server::serv(uint16_t port, int nthreads) {
  instance.listen( "0.0.0.0", port );
  instance.run( nthreads );

  return false; // never return
}

} // mprpc
} // common
} // jubatus
