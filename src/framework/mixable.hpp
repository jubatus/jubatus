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

#pragma once

#include <string>
#include <pficommon/lang/function.h>
#include <iostream>
#include <msgpack.hpp>
#include "../common/exception.hpp"
#include "../config.hpp"

using pfi::lang::function;

namespace jubatus{
namespace framework{

class mixable0 {
public:
  mixable0(){};
  virtual ~mixable0(){};
  virtual std::string get_diff()const = 0;
  virtual void put_diff(const std::string&) = 0;
  virtual void reduce(const std::string&, std::string&) const = 0;
  virtual void save(std::ostream & ofs) = 0;
  virtual void load(std::istream & ifs) = 0;
  virtual void clear() = 0;
};

template <typename Model, typename Diff>
class mixable : public mixable0 {
public:
  mixable():
    get_diff_fun_(&dummy_get_diff),
    reduce_fun_(&dummy_reduce),
    put_diff_fun_(&dummy_put_diff)
  {};
  virtual ~mixable(){};

  virtual void clear() = 0;
  void set_model(pfi::lang::shared_ptr<Model> m){
    model_ = m;
  }

  std::string get_diff()const{
    msgpack::sbuffer sbuf;
    if(model_){
      std::string buf;
      pack_(get_diff_fun_(model_.get()), buf);
      return buf;
    }else{
      throw config_not_set();
    }
  };
  void put_diff(const std::string& d){
    if(model_){
      Diff diff;
      unpack_(d, diff);
      put_diff_fun_(model_.get(), diff);
    }else{
      throw config_not_set();
    }
  }
  void reduce(const std::string& lhs, std::string& acc) const {
    Diff l, a; //<- string
    unpack_(lhs, l);
    unpack_(acc, a);
    reduce_fun_(model_.get(), l, a);
    pack_(a, acc);
  }
  void save(std::ostream & os){
    model_->save(os);
  }
  void load(std::istream & is){
    model_->load(is);
  }

  void set_mixer(function<Diff(const Model*)> get_diff_fun, //get_diff
                 function<int(const Model*, const Diff&, Diff&)> reduce_fun, //mix
                 function<int(Model*, const Diff&)> put_diff_fun //put_diff
                 ) {
    get_diff_fun_ = get_diff_fun;
    reduce_fun_ = reduce_fun;
    put_diff_fun_ = put_diff_fun;
  };
  pfi::lang::shared_ptr<Model> get_model()const{return model_;};

  static Diff dummy_get_diff(const Model*){ return Diff(); };
  static int dummy_reduce(const Model*, const Diff&, Diff&){return -1;};
  static int dummy_put_diff(Model*, const Diff&){return -1;};
private:
  void unpack_(const std::string& buf, Diff& d) const {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.c_str(), buf.size());
    msg.get().convert(&d);
  }
  void pack_(const Diff& d, std::string& buf) const {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, d);
    buf = std::string(sbuf.data(), sbuf.size());
  }

  function<Diff(const Model*)> get_diff_fun_;
  function<int(const Model*, const Diff&, Diff&)> reduce_fun_;
  function<int(Model*, const Diff&)> put_diff_fun_;

  pfi::lang::shared_ptr<Model> model_;

};

template <typename Mixable>
mixable0* mixable_cast(Mixable* m){
  if(m){
    return reinterpret_cast<mixable0*>(m);
  }else{
    throw std::runtime_error("nullpointer exception");
  }
}

} //server
} //jubatus
