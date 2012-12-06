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

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/rwmutex.h>
#include <msgpack.hpp>

#include "../common/exception.hpp"
#include "../common/shared_ptr.hpp"
#include "../common/mprpc/byte_buffer.hpp"

namespace jubatus {
namespace framework {


class mixable0 {
public:
  mixable0() {}
  virtual ~mixable0() {}

  virtual void save(std::ostream & ofs) = 0;
  virtual void load(std::istream & ifs) = 0;
  virtual void clear() = 0;
};


class model_bundler;

class mixable_holder {
public:
  mixable_holder(pfi::lang::shared_ptr<model_bundler> b)
    : bundler_(b)
  {}
  virtual ~mixable_holder() {}

  model_bundler* get_bundler() const {
    return bundler_.get();
  }

  pfi::concurrent::rw_mutex& rw_mutex() {
    return rw_mutex_;
  }

protected:
  pfi::concurrent::rw_mutex rw_mutex_;
  pfi::lang::shared_ptr<model_bundler> bundler_;
};

// strategy: diff
template <typename Model, typename Diff>
class mixable : public mixable0 {
 public:
  typedef Model model_type;
  typedef Diff diff_type;
  typedef common::cshared_ptr<Model> model_ptr;

  virtual ~mixable() {}

  virtual void clear() = 0;

  virtual Diff get_diff_impl() const = 0;
  virtual void put_diff_impl(const Diff&) = 0;
  virtual void mix_impl(const Diff&, const Diff&, Diff&) const = 0;

  void set_model(model_ptr m){
    model_ = m;
  }

  void save(std::ostream & os){
    model_->save(os);
  }

  void load(std::istream & is){
    model_->load(is);
  }

  model_ptr get_model() const { return model_; }

private:
  model_ptr model_;
};


} // framework
} // jubatus

#include "model_bundler.hpp"
