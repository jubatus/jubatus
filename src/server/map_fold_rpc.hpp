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

#include <vector>
#include <map>
#include <string>
#include <exception>

#include <pficommon/lang/function.h>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/network/mprpc.h>

#include <glog/logging.h>

namespace jubatus{

template <typename In, typename Out>
class map_fold_rpc : public pfi::lang::noncopyable{
public:
  map_fold_rpc(){};
  virtual ~map_fold_rpc(){};
  
  // FIXME: not enough functional ... types in C++ is difficult
  // 0: success, positive number: fail
  virtual int call(const std::string& name, const In&, Out&) = 0;
  
};

template <typename In, typename Out>
class map_fold_rpc_sync : protected map_fold_rpc<In,Out>{ //sync version
public:
  typedef void (*fold_func_t)(Out&, const Out&);

  map_fold_rpc_sync(const std::vector<std::pair<std::string,int> >& hosts,
                    double timeout_sec,
                    fold_func_t fold_func):
    map_fold_rpc<In,Out>(),
    hosts_(hosts),
    timeout_sec_(timeout_sec),
    count_(hosts.size()),
    fold_func_(fold_func)
  {
  };
  virtual ~map_fold_rpc_sync(){};
  
  int call(const std::string& name, const In& in,
           Out& out){
    std::vector<std::pair<std::string,int> >::const_iterator it;
    int r = 0;
    for(it = hosts_.begin(); it != hosts_.end(); ++it){
      try{
        pfi::network::mprpc::rpc_client c(it->first, it->second, timeout_sec_);// FIXME: use async call
        typename pfi::lang::function<Out(In)> f = c.call<Out(In)>(name);
        Out rhs = f(in);
        (*fold_func_)(out, rhs); //FIXME out += f(in); didn't work
      }catch(std::exception& e){
        LOG(ERROR) << e.what() << it->first << ":" << it->second;
        r++;
      }
    }
    return r;
  };

 private:

  std::vector<std::pair<std::string,int> > hosts_;
  double timeout_sec_;
  int count_;
  fold_func_t fold_func_;

};

}// jubatus
