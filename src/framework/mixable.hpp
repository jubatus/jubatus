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

#pragma once

#include <string>
#include <iostream>
#include <msgpack.hpp>

#include "../common/exception.hpp"
#include "../common/shared_ptr.hpp"

namespace jubatus{
namespace framework{

class mixable0 {
public:
  mixable0() {}
  virtual ~mixable0() {}
  virtual std::string get_diff() const = 0;
  virtual void put_diff(const std::string&) = 0;
  virtual void mix(const std::string&, const std::string&, std::string&) const = 0;
  virtual void save(std::ostream & ofs) = 0;
  virtual void load(std::istream & ifs) = 0;
  virtual void clear() = 0;
};

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

  std::string get_diff()const{
    if(model_){
      std::string buf;
      pack_(get_diff_impl(), buf);
      return buf;
    }else{
      throw JUBATUS_EXCEPTION(config_not_set());
    }
  };

  void put_diff(const std::string& d){
    if(model_){
      Diff diff;
      unpack_(d, diff);
      put_diff_impl(diff);
    }else{
      throw JUBATUS_EXCEPTION(config_not_set());
    }
  }

  void mix(const std::string& lhs, const std::string& rhs,
           std::string& mixed_string) const {
    Diff left, right, mixed;
    unpack_(lhs, left);
    unpack_(rhs, right);
    mix_impl(left, right, mixed);
    pack_(mixed, mixed_string);
  }

  void save(std::ostream & os){
    model_->save(os);
  }

  void load(std::istream & is){
    model_->load(is);
  }

  model_ptr get_model() const { return model_; }

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

  model_ptr model_;
};

} //server
} //jubatus
