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

class model_diff_mixer_base : public model_mixer_base {
public:
  virtual ~model_diff_mixer_base() {}

  virtual common::mprpc::byte_buffer get_diff() const = 0;
  virtual void put_diff(const common::mprpc::byte_buffer& buf) = 0;
  virtual common::mprpc::byte_buffer mixed_raw_data(const msgpack::object& obj) = 0;
  virtual void mix(const common::mprpc::byte_buffer& lhs_buf, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;
  virtual void mix(const msgpack::object& lhs, const common::mprpc::byte_buffer& rhs_buf, common::mprpc::byte_buffer& mixed) const = 0;

  class mix_impl_base_type { // inherit?
  public:
    //typedef model_diff_mixer_base mix_base_type;
    typedef std::vector<pfi::lang::shared_ptr<mix_impl_base_type> > base_list_type;

    mix_impl_base_type() {}
    ~mix_impl_base_type() {}
    virtual void mix(const msgpack::object& lhs, const msgpack::object& rhs, msgpack::sbuffer& mixed) const = 0;
    virtual void get_diff(msgpack::sbuffer& pk) const = 0;
    virtual void put_diff(const msgpack::object& diff_obj) = 0;
    virtual mixable0* get_mixable() const = 0;
  };
};

// strategy: diff
template <typename Model, typename Diff>
//class diff_mixable : public mixable0 {
class mixable : public mixable0 {
public:
  typedef Model model_type;
  typedef Diff diff_type;
  typedef common::cshared_ptr<Model> model_ptr;
  typedef model_diff_mixer_base::mix_impl_base_type mix_impl_base_type;

  virtual ~mixable() {}

  virtual Diff get_diff_impl() const = 0;
  virtual void put_diff_impl(const Diff&) = 0;
  virtual void mix_impl(const Diff&, const Diff&, Diff&) const = 0;

  pfi::lang::shared_ptr<mix_impl_base_type> create_mix_impl_strategy() const {
    //return pfi::lang::shared_ptr<mix_impl_base_type>(new mix_diff_impl(*this));
    return pfi::lang::shared_ptr<mix_impl_base_type>();
  };

  void set_model(model_ptr m) {
    model_ = m;
  }

  void save(std::ostream& os) {
    model_->save(os);
  }

  void load(std::istream& is) {
    model_->load(is);
  }

  model_ptr get_model() const { return model_; }

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

private:
  model_ptr model_;
};


} // framework
} // jubatus

#include "model_bundler.hpp"

