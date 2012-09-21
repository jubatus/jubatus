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

#include "global_id_generator.hpp"
#include <cassert>

namespace jubatus { namespace common {


global_id_generator::global_id_generator():
  is_standalone_(true),
  counter_(0)
{}
global_id_generator::global_id_generator(bool is_standalone):
  is_standalone_(is_standalone),
  counter_(0)
{}

global_id_generator::~global_id_generator()
{}

uint64_t global_id_generator::generate()
{
  if(is_standalone_){
    return __sync_fetch_and_add(&counter_, 1);

  }else{
#ifdef HAVE_ZOOKEEPER_H

    // FIXME: to be implemented
    return ls_->create_id(path_);

#else
    // never reaches here
    assert(is_standalone_);
    return 0; // dummy to remove warning
#endif
  }
}

#ifdef HAVE_ZOOKEEPER_H
    void global_id_generator::set_ls(cshared_ptr<lock_service>& ls,
                                     const std::string& path_prefix)
{
  path_ = path_prefix + "/id_generator";
  ls_ = ls;
  ls_->create(path_);
}
#endif

}}
