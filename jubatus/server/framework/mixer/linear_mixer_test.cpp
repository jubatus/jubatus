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

#include <iostream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "jubatus/core/framework/mixable.hpp"
#include "linear_mixer.hpp"



using std::vector;
using std::string;
using std::istream;
using std::ostream;
using std::stringstream;
using pfi::lang::shared_ptr;
using jubatus::core::common::byte_buffer;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

namespace {

vector<byte_buffer> make_packed_vector(const string& s) {
  vector<byte_buffer> v;
  // pack mix-internal
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, s);
  v.push_back(byte_buffer(sbuf.data(), sbuf.size()));

  return v;
}

common::mprpc::rpc_response_t make_response(const string& s) {
  common::mprpc::rpc_response_t res;
  res.zone = mp::shared_ptr<msgpack::zone>(new msgpack::zone);
  res.response.a3 = msgpack::object(make_packed_vector(s), res.zone.get());

  return res;
}

}  // namespace

class linear_communication_stub : public linear_communication {
 public:
  size_t update_members() {
    return 4;
  }

  pfi::lang::shared_ptr<common::try_lockable> create_lock() {
    return pfi::lang::shared_ptr<common::try_lockable>();
  }

  void get_diff(common::mprpc::rpc_result_object& result) const {
    result.response.push_back(make_response("1"));
    result.response.push_back(make_response("2"));
    result.response.push_back(make_response("3"));
    result.response.push_back(make_response("4"));
  }

  void put_diff(const vector<byte_buffer>& mixed) const {
    mixed_ = mixed;
  }

  const vector<string> get_mixed() const {
    vector<string> mixed;
    for (vector<byte_buffer>::const_iterator it = mixed_.begin();
        it != mixed_.end(); ++it) {
      // unpack mix-internal
      msgpack::unpacked msg;
      msgpack::unpack(&msg, it->ptr(), it->size());
      mixed.push_back(msg.get().as<string>());
    }
    return mixed;
  }

 private:
  mutable vector<byte_buffer> mixed_;
};

struct mixable_string
    : public core::framework::mixable<mixable_string, string> {
 public:
  string get_diff_impl() const {
    return string();
  }
  void put_diff_impl(const string&) {
  }
  void mix_impl(const string& lhs, const string& rhs, string& mixed) const {
    stringstream ss;
    ss << "(" << lhs << "+" << rhs << ")";
    mixed = ss.str();
  }
  void save(ostream&) {
  }
  void load(istream&) {
  }
  void clear() {
  }
};

TEST(linear_mixer, mix_order) {
  shared_ptr<linear_communication_stub> com(new linear_communication_stub);
  linear_mixer m(com, 1, 1);

  pfi::lang::shared_ptr<core::framework::mixable_holder> holder(
      new core::framework::mixable_holder());
  m.set_mixable_holder(holder);

  mixable_string s;
  holder->register_mixable(&s);

  m.mix();

  vector<string> mixed = com->get_mixed();
  ASSERT_EQ(1u, mixed.size());
  EXPECT_EQ("(4+(3+(2+1)))", mixed[0]);
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
