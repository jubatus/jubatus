// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "../../../core/common/version.hpp"
#include "../../../core/common/byte_buffer.hpp"
#include "../../../core/framework/mixable.hpp"
#include "linear_mixer.hpp"
#include "jubatus/core/framework/mixable.hpp"

using std::string;
using std::vector;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
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
  size_t update_members() { return 4; }

  jubatus::util::lang::shared_ptr<common::try_lockable> create_lock() {
    return jubatus::util::lang::shared_ptr<common::try_lockable>();
  }

  void get_diff(common::mprpc::rpc_result_object& result) const {
    result.response.push_back(make_response("1"));
    result.response.push_back(make_response("2"));
    result.response.push_back(make_response("3"));
    result.response.push_back(make_response("4"));
    result.error.push_back(common::mprpc::rpc_error("1", 1));
    result.error.push_back(common::mprpc::rpc_error("2", 2));
    result.error.push_back(common::mprpc::rpc_error("3", 3));
    result.error.push_back(common::mprpc::rpc_error("4", 4));
  }

  void put_diff(const vector<byte_buffer>& mixed,
                common::mprpc::rpc_result_object& result) const {
    vector<string> tmp;
    tmp.reserve(mixed.size());
    typedef vector<byte_buffer>::const_iterator iter_t;
    for (iter_t it = mixed.begin(); it != mixed.end(); ++it) {
      if (const char* p = it->ptr()) {
        size_t size = it->size();
        tmp.push_back(std::string(p, size));
      } else {
        tmp.push_back("");
      }
    }
    mixed_.swap(tmp);
  }

  vector<string> get_mixed() const {
    vector<string> mixed;
    mixed.reserve(mixed_.size());

    typedef vector<string>::const_iterator iter_t;
    for (iter_t it = mixed_.begin(); it != mixed_.end(); ++it) {
      msgpack::unpacked msg;
      msgpack::unpack(&msg, it->data(), it->size());
      mixed.push_back(msg.get().as<string>());
    }

    return mixed;
  }

  byte_buffer get_model() {
    return byte_buffer();
  }

  bool register_active_list() const {
    return true;
  }
  bool unregister_active_list() const {
    return true;
  }

 private:
  mutable vector<string> mixed_;
};

struct mixable_string : public core::framework::deprecated_mixable<
    mixable_string, core::common::byte_buffer> {
 public:
  core::common::byte_buffer get_diff_impl() const {
    return core::common::byte_buffer();
  }
  bool put_diff_impl(const byte_buffer&) {
    return true;
  }
  void mix_impl(
      const byte_buffer& lhs,
      const byte_buffer& rhs,
      byte_buffer& mixed) const {
    std::stringstream ss;
    ss << "(" << string(lhs.ptr(), lhs.size()) << "+"
       << string(rhs.ptr(), rhs.size()) << ")";
    string s = ss.str();
    mixed.assign(s.data(), s.size());
  }
  string get_pull_argument() const { return string(); }
  string pull(const string&) const { return string(); }
  void push(const string&) {}
  void save(std::ostream&) {}
  void load(std::istream&) {}
  core::storage::version get_version() const {
    return core::storage::version();
  }
  void clear() {}
};

TEST(linear_mixer, mix_order) {
  shared_ptr<linear_communication_stub> com(new linear_communication_stub);
  linear_mixer m(com, 1, 1);

  jubatus::util::lang::shared_ptr<core::framework::mixable_holder> holder(
      new core::framework::mixable_holder());
  m.set_mixable_holder(holder);

  jubatus::util::lang::shared_ptr<mixable_string> s(new mixable_string);
  holder->register_mixable(s);

  m.mix();

  vector<string> mixed = com->get_mixed();
  ASSERT_EQ(1u, mixed.size());
  EXPECT_EQ("(4+(3+(2+1)))", mixed[0]);
}

TEST(linear_mixer, destruct_running_mixer) {
  shared_ptr<linear_communication_stub> com(new linear_communication_stub);
  linear_mixer m(com, 1, 1);

  jubatus::util::lang::shared_ptr<core::framework::mixable_holder> holder(
      new core::framework::mixable_holder());
  m.set_mixable_holder(holder);

  jubatus::util::lang::shared_ptr<mixable_string> s(new mixable_string);
  holder->register_mixable(s);

  m.start();

  // destruct without calling m.stop()
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

