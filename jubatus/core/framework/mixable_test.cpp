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

#include "mixable.hpp"

#include <sstream>
#include <gtest/gtest.h>

using std::stringstream;
using jubatus::core::common::byte_buffer;

namespace jubatus {
namespace core {
namespace framework {

struct int_model {
  int_model()
      : value(0) {
  }

  int value;

  MSGPACK_DEFINE(value);

  template<class Buffer>
  void pack(msgpack::packer<Buffer>& packer) const {
    packer.pack(*this);
  }
  void unpack(msgpack::object o) {
    o.convert(this);
  }
};

class mixable_int : public mixable<int_model, int> {
 public:
  mixable_int()
      : diff_() {
  }

  void clear() {
  }

  int get_diff_impl() const {
    return diff_;
  }

  bool put_diff_impl(const int& n) {
    get_model()->value += n;
    diff_ = 0;
    return true;
  }

  void mix_impl(const int& lhs, const int& rhs, int& mixed) const {
    mixed = lhs + rhs;
  }

  void add(int n) {
    diff_ += n;
  }

 private:
  int diff_;
};

TEST(mixable, config_not_set) {
  mixable_int m;
  EXPECT_THROW(m.get_diff(), common::config_not_set);
  EXPECT_THROW(m.put_diff(byte_buffer()), common::config_not_set);
}

TEST(mixable, pack_and_unpack) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));
  m.get_model()->value = 10;

  msgpack::sbuffer buf;
  msgpack::packer<msgpack::sbuffer> packer(buf);
  m.pack(packer);

  m.get_model()->value = 5;

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());
  m.unpack(unpacked.get());

  EXPECT_EQ(10, m.get_model()->value);
}

TEST(mixable, trivial) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));

  m.add(10);

  byte_buffer diff1 = m.get_diff();
  byte_buffer diff2 = m.get_diff();

  byte_buffer mixed;
  m.mix(diff1, diff2, mixed);

  m.put_diff(mixed);

  EXPECT_EQ(20, m.get_model()->value);
}

}  // namespace framework
}  // namespace core
}  // namespace jubatus
