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
struct object_holder : public msgpack::object {
  template <typename T>
  object_holder(const T& v, msgpack::zone* z)
    : msgpack::object(v, z)
  {
  }
  object_holder()
  {
  }
};
}
}
namespace msgpack {
inline void operator<< (msgpack::object::with_zone& o, const jubatus::framework::object_holder& v)
{
  o.type = v.type;
  o.via = v.via;
}
}

namespace jubatus{
namespace framework{


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


namespace detail {

class mix_wrapper_base {
public:
  virtual ~mix_wrapper_base()
  {
  }
  virtual void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) = 0;
  virtual void get_diff(msgpack::sbuffer& pk) = 0;
  virtual void put_diff(const msgpack::object& diff_obj) = 0;
  virtual mixable0* mixable() const = 0;
};

template <class M>
class mix_wrapper : public detail::mix_wrapper_base {
public:
  mix_wrapper(M& m)
    : m_(m)
  {
  }

  void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) {
    typename M::diff_type lhs_diff, rhs_diff, m;
    lhs.convert(&lhs_diff);
    rhs.convert(&rhs_diff);
    m_.mix_impl(lhs_diff, rhs_diff, m);

    // output packed data
    msgpack::pack(mixed, m);
  }

  void get_diff(msgpack::sbuffer& pk) {
    msgpack::pack(pk, m_.get_diff_impl());
  }

  void put_diff(const msgpack::object& diff_obj) {
    typename M::diff_type diff;
    diff_obj.convert(&diff);
    m_.put_diff_impl(diff);
  }

  mixable0* mixable() const {
    return &m_;
  }

protected:
  M& m_;
};
} // detail

class model_bundler : public mixable0 {
public:
  typedef std::vector<msgpack::object> diff_object;
  typedef std::vector<pfi::lang::shared_ptr<detail::mix_wrapper_base> >  wrapper_list;

  template <class M1 /*= mixable */>
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1)
  {
    model_bundler::wrapper_list m;
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M1>(m1)));
    return pfi::lang::shared_ptr<model_bundler>(new model_bundler(m));
  }

  template <class M1, class M2>
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1, M2& m2)
  {
    model_bundler::wrapper_list m;
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M1>(m1)));
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M2>(m2)));
    return pfi::lang::shared_ptr<model_bundler>(new model_bundler(m));
  }

  template <class M1, class M2, class M3>
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1, M2& m2, M3& m3)
  {
    model_bundler::wrapper_list m;
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M1>(m1)));
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M2>(m2)));
    m.push_back(pfi::lang::shared_ptr<detail::mix_wrapper_base>(new detail::mix_wrapper<M3>(m3)));
    return pfi::lang::shared_ptr<model_bundler>(new model_bundler(m));
  }

private:
  model_bundler(const wrapper_list& mix_wrappers)
    : m_(mix_wrappers)
  {
  }
public:

  common::mprpc::byte_buffer get_diff() const
  {
    // diff buffer
    msgpack::sbuffer diff_buf;
    msgpack::packer<msgpack::sbuffer> pk(&diff_buf);

    // TODO: create format/type
    pk.pack_array(m_.size());

    for (wrapper_list::const_iterator it = m_.begin(), end = m_.end();
        it != end; ++it) {
      (*it)->get_diff(diff_buf);
    }

    return common::mprpc::byte_buffer(diff_buf.data(), diff_buf.size());
  }

  void put_diff(const common::mprpc::byte_buffer& buf)
  {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.ptr(), buf.size());

    // TODO: 1. check format: diff_object
    diff_object diff_obj;
    msg.get().convert(&diff_obj);

    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->put_diff(diff_obj[i]);
    }
  }

  common::mprpc::byte_buffer mixed_raw_data(const msgpack::object& obj)
  {
    msgpack::sbuffer sbuf;
    msgpack::pack(&sbuf, obj);
    return common::mprpc::byte_buffer(sbuf.data(), sbuf.size());
  }

  void mix(const common::mprpc::byte_buffer& lhs, const common::mprpc::byte_buffer& rhs, common::mprpc::byte_buffer& mixed) const
  {
    // unpacker for lhs and rhs
    msgpack::unpacked lhs_msg, rhs_msg;
    // TODO return result
    msgpack::unpack(&lhs_msg, lhs.ptr(), lhs.size());
    msgpack::unpack(&rhs_msg, rhs.ptr(), rhs.size());

    diff_object lhs_obj, rhs_obj;
    lhs_msg.get().convert(&lhs_obj);
    rhs_msg.get().convert(&rhs_obj);

    // mixed diff buffer
    msgpack::sbuffer mixed_buf;
    //msgpack::packer<msgpack::sbuffer> mixed_packer(&mixed_buf);
    // TODO: create format/type

    // mix
    mix_core(lhs_obj, rhs_obj, mixed_buf);

    mixed.assign(mixed_buf.data(), mixed_buf.size());
  }

  void mix(const msgpack::object& lhs, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const
  {
    msgpack::unpacked rhs_msg;
    // TODO return result
    msgpack::unpack(&rhs_msg, rhs_buf.ptr(), rhs_buf.size());

    // TODO: 1. check format: diff_object
    diff_object lhs_obj, rhs_obj;
    lhs.convert(&lhs_obj);
    rhs_msg.get().convert(&rhs_obj);

    // 2. check size of rhs and lhs (array)
    // 2. unpack lhs and rhs

    // mixed diff buffer
    msgpack::sbuffer mixed_buf;
    //msgpack::packer<msgpack::sbuffer> mixed_packer(&mixed_buf);
    // TODO: create format/type

    // mix
    mix_core(lhs_obj, rhs_obj, mixed_buf);

    mixed = common::mprpc::byte_buffer(mixed_buf.data(), mixed_buf.size());
  }

protected:

  void mix_core(const diff_object& lhs_obj, const diff_object& rhs_obj,
      msgpack::sbuffer& mixed_buf) const
  {
    // TODO: 1. check format: diff_object

    // assert(lhs_obj.size() == rhs_obj.size() == m_.size());
    // TODO: create format/type
    // mixed_paker << ...;
    msgpack::packer<msgpack::sbuffer> mixed_packer(&mixed_buf);
    mixed_packer.pack_array(m_.size());

    // 2. check size of rhs and lhs (array)
    // 2. unpack lhs and rhs

    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->mix(lhs_obj[i], rhs_obj[i], mixed_buf);
    }
  }

public:

  void save(std::ostream & ofs)
  {
    for (wrapper_list::const_iterator it = m_.begin(), end = m_.end();
        it != end; ++it) {
      (*it)->mixable()->save(ofs);
    }
  }

  void load(std::istream & ifs)
  {
    for (wrapper_list::const_iterator it = m_.begin(), end = m_.end();
        it != end; ++it) {
      (*it)->mixable()->load(ifs);
    }
  }

  void clear()
  {
  }

protected:
  wrapper_list m_;
};


} //server
} //jubatus
