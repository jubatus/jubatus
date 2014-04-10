// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixable_helper.hpp"
#include "stream_writer.hpp"

#include <sstream>
#include <gtest/gtest.h>
#include "../../util/lang/shared_ptr.h"
#include "../common/version.hpp"

using std::stringstream;
using jubatus::core::common::byte_buffer;

namespace jubatus {
namespace core {
namespace framework {

struct int_model : public model {
  int_model()
      : value(0), diff_(0) {
  }

  int value;

  // TODO(suma): replace packer
  void pack(msgpack::packer<msgpack::sbuffer>& packer) const {
    packer.pack(value);
  }

  void unpack(msgpack::object o) {
    o.convert(&value);
  }

  void clear() {
  }

  // for linear_mixable
  void get_diff(int& diff) const {
    diff = diff_;
  }

  bool put_diff(const int& n) {
    value += n;
    diff_ = 0;
    return true;
  }

  void mix(const int& lhs, int& mixed) const {
    mixed += lhs;
  }

  void add(int n) {
    diff_ += n;
  }

 private:
  int diff_;
};

typedef linear_mixable_helper<int_model, int> mixable_int;

TEST(mixable, config_not_set) {
  //EXPECT_THROW(mixable_int(),common::config_not_set);
}

TEST(mixable, pack_unpack) {
  mixable_int m(mixable_int::model_ptr(new int_model));
  m.get_model()->value = 10;

  msgpack::sbuffer buf;
  msgpack::packer<msgpack::sbuffer> pk(buf);
  m.get_model()->pack(pk);

   m.get_model()->value = 5;

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());
  m.get_model()->unpack(unpacked.get());

   EXPECT_EQ(10, m.get_model()->value);
}

TEST(mixable, trivial) {
  mixable_int m(mixable_int::model_ptr(new int_model));

  m.get_model()->add(10);

  msgpack::sbuffer diff1, diff2;
  stream_writer<msgpack::sbuffer> sw1(diff1), sw2(diff2);
  core::framework::msgpack_packer mp1(sw1), mp2(sw2);
  packer pk1(mp1), pk2(mp2);
  m.get_diff(pk1);
  m.get_diff(pk2);

  msgpack::unpacked m1, m2;
  msgpack::unpack(&m1, diff1.data(), diff1.size());
  msgpack::unpack(&m2, diff2.data(), diff2.size());

  diff_object diff_obj_mixed = m.convert_diff_object(m1.get());
  m.mix(m2.get(), diff_obj_mixed);
  m.put_diff(diff_obj_mixed);

  EXPECT_EQ(20, m.get_model()->value);
}

}  // namespace framework
}  // namespace core
}  // namespace jubatus
