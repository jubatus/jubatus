// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixable.hpp"

namespace jubatus {
namespace framework {

class model_diff_mixer_base : public model_bundler {
public:
  virtual ~model_diff_mixer_base() {}

  virtual common::mprpc::byte_buffer get_diff() const = 0;
  virtual void put_diff(const common::mprpc::byte_buffer& buf) = 0;
  virtual void mix(const common::mprpc::byte_buffer& lhs_buf, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;
  virtual void mix(const msgpack::object& lhs, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;

  //virtual common::mprpc::byte_buffer mixed_raw_data(const msgpack::object& obj) = 0;
  static common::mprpc::byte_buffer mixed_raw_data(const msgpack::object& obj)
  {
    msgpack::sbuffer sbuf;
    msgpack::pack(&sbuf, obj);
    return common::mprpc::byte_buffer(sbuf.data(), sbuf.size());
  }


  class mix_impl_base_type {
  public:
    typedef std::vector<pfi::lang::shared_ptr<mix_impl_base_type> > base_list_type;

    mix_impl_base_type() {}
    virtual ~mix_impl_base_type() {}
    virtual void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) const = 0;
    virtual void get_diff(msgpack::sbuffer& pk) const = 0;
    virtual void put_diff(const msgpack::object& diff_obj) = 0;
    virtual mixable0* get_mixable() const = 0;
  };
};

template <typename Model, typename Diff>
// TODO: rename `diff_mixable`
class mixable : public model_holder<Model> {
/* or
//class diff_mixable : public mixable0 {};
//class nantoka_mixer : public diff_mixable<M, D>, public mixable_holder<M> {};
 */
public:
  typedef Model model_type;
  typedef Diff diff_type;
  typedef model_diff_mixer_base::mix_impl_base_type mix_impl_base_type;

  virtual ~mixable() {}

  virtual Diff get_diff_impl() const = 0;
  virtual void put_diff_impl(const Diff&) = 0;
  virtual void mix_impl(const Diff&, const Diff&, Diff&) const = 0;

  pfi::lang::shared_ptr<mix_impl_base_type> create_mix_impl_strategy() {
    return pfi::lang::shared_ptr<mix_impl_base_type>(new mix_diff_impl(*this));
  }

protected:
  class mix_diff_impl : public mix_impl_base_type {
  public:
    mix_diff_impl(mixable<Model, Diff> & m)
      : m_(m)
    {
    }

    void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) const {
      diff_type lhs_diff, rhs_diff, m;
      lhs.convert(&lhs_diff);
      rhs.convert(&rhs_diff);
      m_.mix_impl(lhs_diff, rhs_diff, m);

      // output packed data
      msgpack::pack(mixed, m);
    }

    void get_diff(msgpack::sbuffer& pk) const {
      msgpack::pack(pk, m_.get_diff_impl());
    }

    void put_diff(const msgpack::object& diff_obj) {
      diff_type diff;
      diff_obj.convert(&diff);
      m_.put_diff_impl(diff);
    }

    mixable0* get_mixable() const {
      return &m_;
    }

  protected:
    mixable<Model, Diff> & m_;
  };
};


class diff_model_bundler : public bundler_factory<diff_model_bundler, model_diff_mixer_base> {
public:
  diff_model_bundler(const mix_func_list& m)
    : bundler_factory<diff_model_bundler, model_diff_mixer_base>(m)
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


} // framework
} // jubatus

