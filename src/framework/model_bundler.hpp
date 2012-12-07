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

#include <vector>
#include <msgpack.hpp>
#include <pficommon/lang/shared_ptr.h>

#include "mixable.hpp"
#include "../common/mprpc/byte_buffer.hpp"


namespace jubatus {
namespace framework {


#if 0
namespace detail {

class mix_policy_base {
public:
  virtual ~mix_policy_base()
  {
  }
};

// interface
class model_mixer_base {
public:
  virtual ~model_mixer_base() {}
};

class model_diff_mixer_base : public model_mixer_base {
public:
  virtual ~model_diff_mixer_base() {}

  virtual common::mprpc::byte_buffer get_diff() const = 0;
  virtual void put_diff(const common::mprpc::byte_buffer& buf) = 0;
  virtual common::mprpc::byte_buffer mixed_raw_data(const msgpack::object& obj) = 0;
  virtual void mix(const common::mprpc::byte_buffer& lhs_buf, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;
  virtual void mix(const msgpack::object& lhs, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;

  //virtual mixable0* mixable() const = 0;
};

class strategy_bundler_base {
public:
  virtual ~strategy_bundler_base() {}
};

class mix_diff_policy : public mix_policy_base {
public:
  typedef model_diff_mixer_base mixer_base_type;
  typedef std::vector<pfi::lang::shared_ptr<mix_diff_policy> > base_list_type;

  mix_diff_policy() {}
  ~mix_diff_policy() {}
  virtual void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) = 0;
  virtual void get_diff(msgpack::sbuffer& pk) = 0;
  virtual void put_diff(const msgpack::object& diff_obj) = 0;
  virtual mixable0* mixable() const = 0;
};

template <class M>
class mix_diff_impl : public mix_diff_policy {
public:
  mix_diff_impl(M& m)
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



template <class T,  // CRTP
         class P,  // mix policy
         class B,  // mix base class
         template <class> class S > // mix_impl with <Model>
class strategy_bundler : public B {
public:
  typedef P impl_base_type;
  typedef typename P::base_list_type mix_func_list;

  template <class Strategy>
  bool is_mix() const {
    return dynamic_cast<Strategy*>(this);
  }

  // factory method
  template <class M1> // M1 = mixable
  static pfi::lang::shared_ptr<T> create(M1& m1)
  {
    mix_func_list m;
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M1>(m1)));
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  template <class M1, class M2>
  static pfi::lang::shared_ptr<T> create(M1& m1, M2& m2)
  {
    mix_func_list m;
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M1>(m1)));
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M2>(m2)));
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  template <class M1, class M2, class M3>
  static pfi::lang::shared_ptr<T> create(M1& m1, M2& m2, M3& m3)
  {
    mix_func_list m;
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M1>(m1)));
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M2>(m2)));
    m.push_back(pfi::lang::shared_ptr<impl_base_type>(new S<M3>(m3)));
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  // basic functions
  void save(std::ostream & ofs)
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->mixable()->save(ofs);
    }
  }

  void load(std::istream & ifs)
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->mixable()->clear();
      m_[i]->mixable()->load(ifs);
    }
  }

  void clear()
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->mixable()->clear();
    }
  }

  strategy_bundler(const mix_func_list& m)
    : m_(m)
  {
  }

protected:
  mix_func_list m_;
};


} // detail
#endif

template <class T, class B>
class factory : public B {
public:
  typedef typename B::mix_impl_base_type impl_base_type;;
  typedef typename B::mix_impl_base_type::base_list_type mix_func_list;;
  //typedef std::vector<pfi::lang::shared_ptr<impl_base_type> > mix_func_list;

  // factory method
  template <class M1> // M1 = template diff_mixable
  static pfi::lang::shared_ptr<T> create(M1& m1)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  template <class M1, class M2>
  static pfi::lang::shared_ptr<T> create(M1& m1, M2& m2)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    m.push_back(m2.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  template <class M1, class M2, class M3>
  static pfi::lang::shared_ptr<T> create(M1& m1, M2& m2, M3& m3)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    m.push_back(m2.create_mix_impl_strategy());
    m.push_back(m3.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  factory(const mix_func_list& m)
    : m_(m)
  {
  }

  void save(std::ostream & ofs)
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->get_mixable()->save(ofs);
    }
  }

  void load(std::istream & ifs)
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->get_mixable()->clear();
      m_[i]->get_mixable()->load(ifs);
    }
  }

  void clear()
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->get_mixable()->clear();
    }
  }

protected:
  mix_func_list m_;
};

//class diff_model_bundler : public factory<diff_model_bundler, model_diff_mixer_base> {
class model_bundler : public factory<model_bundler, model_diff_mixer_base> {
public:
  model_bundler(const mix_func_list& m)
    : factory<model_bundler, model_diff_mixer_base>(m)
  {
  }
  typedef std::vector<msgpack::object> diff_object;

public:
  common::mprpc::byte_buffer get_diff() const
  {
    // diff buffer
    msgpack::sbuffer diff_buf;
    msgpack::packer<msgpack::sbuffer> pk(&diff_buf);

    // TODO: create format/type
    pk.pack_array(m_.size());

    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->get_diff(diff_buf);
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

  void mix(const common::mprpc::byte_buffer& lhs_buf, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const
  {
    // preparing object
    msgpack::unpacked lhs_msg, rhs_msg;
    msgpack::unpack(&lhs_msg, lhs_buf.ptr(), lhs_buf.size());
    msgpack::unpack(&rhs_msg, rhs_buf.ptr(), rhs_buf.size());
    msgpack::object lhs = lhs_msg.get();
    msgpack::object rhs = rhs_msg.get();

    // mix
    msgpack::sbuffer mixed_buf;
    mix_core(lhs, rhs, mixed_buf);

    mixed.assign(mixed_buf.data(), mixed_buf.size());
  }

  void mix(const msgpack::object& lhs, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const
  {
    // preparing object
    msgpack::unpacked rhs_msg;
    msgpack::unpack(&rhs_msg, rhs_buf.ptr(), rhs_buf.size());
    msgpack::object rhs = rhs_msg.get();

    // mix
    msgpack::sbuffer mixed_buf;
    mix_core(lhs, rhs, mixed_buf);

    mixed = common::mprpc::byte_buffer(mixed_buf.data(), mixed_buf.size());
  }

protected:

  void mix_core(const msgpack::object& lhs_obj, const msgpack::object& rhs_obj,
      msgpack::sbuffer& mixed_buf) const
  {
    // TODO: 1. check format: diff_object
    diff_object lhs, rhs;
    lhs_obj.convert(&lhs);
    rhs_obj.convert(&rhs);

    // assert(lhs.size() == rhs.size() == m_.size());
    // TODO: create format/type
    // mixed_paker << ...;
    msgpack::packer<msgpack::sbuffer> mixed_packer(&mixed_buf);
    mixed_packer.pack_array(m_.size());

    // 2. check size of rhs and lhs (array)
    // 2. unpack lhs and rhs

    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->mix(lhs[i], rhs[i], mixed_buf);
    }
  }
};


// strategy_bundler


} // framework
} // jubatus
