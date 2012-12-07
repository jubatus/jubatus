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

template <class T, class B>
class factory : public B {
public:
  typedef typename B::mix_impl_base_type impl_base_type;;
  typedef typename B::mix_impl_base_type::base_list_type mix_func_list;;

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

  void save(std::ostream& ofs)
  {
    for (size_t i = 0, size = m_.size(); i < size; ++i) {
      m_[i]->get_mixable()->save(ofs);
    }
  }

  void load(std::istream& ifs)
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
