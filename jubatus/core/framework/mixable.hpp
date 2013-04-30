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

#ifndef JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_
#define JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_

#include <iostream>
#include <string>
#include <vector>

#include <msgpack.hpp>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/lang/shared_ptr.h>
#include "../common/exception.hpp"
#include "../common/byte_buffer.hpp"

namespace jubatus {
namespace core {
namespace framework {

class mixable0 {
 public:
  mixable0() {
  }

  virtual ~mixable0() {
  }

  virtual common::byte_buffer get_diff() const = 0;
  virtual void put_diff(const common::byte_buffer&) = 0;
  virtual void mix(const common::byte_buffer&,
                   const common::byte_buffer&,
                   common::byte_buffer&) const = 0;

  virtual void save(std::ostream& ofs) = 0;
  virtual void load(std::istream& ifs) = 0;
  virtual void clear() = 0;
};

class mixable_holder {
 public:
  typedef std::vector<mixable0*> mixable_list;

  mixable_holder() {
  }

  virtual ~mixable_holder() {
  }

  void register_mixable(mixable0* m) {
    mixables_.push_back(m);
  }

  mixable_list get_mixables() const {
    return mixables_;
  }

  pfi::concurrent::rw_mutex& rw_mutex() {
    return rw_mutex_;
  }

 protected:
  pfi::concurrent::rw_mutex rw_mutex_;
  std::vector<mixable0*> mixables_;
};

template<typename Model, typename Diff>
class mixable : public mixable0 {
 public:
  typedef Model model_type;
  typedef Diff diff_type;
  typedef pfi::lang::shared_ptr<Model> model_ptr;

  virtual ~mixable() {
  }

  virtual void clear() = 0;

  virtual Diff get_diff_impl() const = 0;
  virtual void put_diff_impl(const Diff&) = 0;
  virtual void mix_impl(const Diff&, const Diff&, Diff&) const = 0;

  void set_model(model_ptr m) {
    model_ = m;
  }

  common::byte_buffer get_diff() const {
    if (model_) {
      common::byte_buffer buf;
      pack_(get_diff_impl(), buf);
      return buf;
    } else {
      throw JUBATUS_EXCEPTION(config_not_set());
    }
  }

  void put_diff(const common::byte_buffer& d) {
    if (model_) {
      Diff diff;
      unpack_(d, diff);
      put_diff_impl(diff);
    } else {
      throw JUBATUS_EXCEPTION(config_not_set());
    }
  }

  void mix(
      const common::byte_buffer& lhs,
      const common::byte_buffer& rhs,
      common::byte_buffer& mixed_buf) const {
    Diff left, right, mixed;
    unpack_(lhs, left);
    unpack_(rhs, right);
    mix_impl(left, right, mixed);
    pack_(mixed, mixed_buf);
  }

  void save(std::ostream& os) {
    model_->save(os);
  }

  void load(std::istream& is) {
    model_->load(is);
  }

  model_ptr get_model() const {
    return model_;
  }

 private:
  void unpack_(const common::byte_buffer& buf, Diff& d) const {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.ptr(), buf.size());
    msg.get().convert(&d);
  }

  void pack_(const Diff& d, common::byte_buffer& buf) const {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, d);
    buf.assign(sbuf.data(), sbuf.size());
  }

  model_ptr model_;
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_
