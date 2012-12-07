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

class model_bundler {
public:
  virtual ~model_bundler()
  {
  }
  virtual void save(std::ostream& ofs) = 0;
  virtual void load(std::istream& ifs) = 0;
  virtual void clear() = 0;
};

template <class T, class B>
class bundler_factory : public B {
public:
  typedef typename B::mix_impl_base_type impl_base_type;;
  typedef typename B::mix_impl_base_type::base_list_type mix_func_list;;

  // factory method
  template <class M1> // M1 = template diff_mixable
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<T>(new T(m));
  }

  template <class M1, class M2>
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1, M2& m2)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    m.push_back(m2.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<model_bundler>(new T(m));
  }

  template <class M1, class M2, class M3>
  static pfi::lang::shared_ptr<model_bundler> create(M1& m1, M2& m2, M3& m3)
  {
    mix_func_list m;
    m.push_back(m1.create_mix_impl_strategy());
    m.push_back(m2.create_mix_impl_strategy());
    m.push_back(m3.create_mix_impl_strategy());
    return pfi::lang::shared_ptr<model_bundler>(new T(m));
  }

  bundler_factory(const mix_func_list& m)
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

} // framework
} // jubatus
