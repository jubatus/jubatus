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
#include "../../../core/framework/mixable_helper.hpp"
#include "linear_mixer.hpp"
#include "jubatus/core/framework/mixable.hpp"

using std::string;
using std::vector;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::common::byte_buffer;

using namespace std;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {
namespace {

vector<string> make_packed_vector(const string& s) {
    vector<string> v;
    // pack mix-internal
    v.push_back(s);
    return v;
}

common::mprpc::rpc_response_t make_response(const string& s) {
  common::mprpc::rpc_response_t res;
  res.zone = mp::shared_ptr<msgpack::zone>(new msgpack::zone);
  res.response.a3 = msgpack::object(make_packed_vector(s), res.zone.get());
  cout << "response: " << res.response.a3 << endl;

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
    cout << "get_diff called" << endl;
    result.response.push_back(make_response("1"));
    result.response.push_back(make_response("2"));
    result.response.push_back(make_response("3"));
    result.response.push_back(make_response("4"));
    result.error.push_back(common::mprpc::rpc_error("1", 1));
    result.error.push_back(common::mprpc::rpc_error("2", 2));
    result.error.push_back(common::mprpc::rpc_error("3", 3));
    result.error.push_back(common::mprpc::rpc_error("4", 4));
  }

  void put_diff(const byte_buffer& mixed,
                common::mprpc::rpc_result_object& result) const {
    cout << "put_diff " << mixed.size() << endl;

    msgpack::unpacked msg;
    msgpack::unpack(&msg, mixed.ptr(), mixed.size());
    vector<string> tmp = msg.get().as<vector<string> >();
    mixed_.swap(tmp);

    cout << "msg:" << msg.get() << endl;

    /*
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(sbuf);
    pk.pack_array(1);
    pk.pack(mixed);
    vector<string> tmp;

    tmp.reserve(mixed.size());
    typedef vector<byte_buffer>::const_iterator iter_t;
    for (iter_t it = mixed.begin(); it != mixed.end(); ++it) {
      if (const char* p = it->ptr()) {
        size_t size = it->size();
        tmp.push_back(string(p, size));
      } else {
        tmp.push_back("");
      }
    }
    mixed_.swap(tmp);
  */
  }

  vector<string> get_mixed() const {
    return mixed_;
#if 0
    vector<string> mixed;
    mixed.reserve(mixed_.size());

    typedef vector<string>::const_iterator iter_t;
    for (iter_t it = mixed_.begin(); it != mixed_.end(); ++it) {
      msgpack::unpacked msg;
      msgpack::unpack(&msg, it->data(), it->size());
      mixed.push_back(msg.get().as<string>());
    }

    return mixed;
#endif
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

struct my_string {
 public:
   void get_diff(string& diff) const {
    diff = string();
  }
  bool put_diff(const string&) {
    return true;
  }
  void mix( const string& lhs, string& mixed) const {
    cout << "lhs: "<< lhs << endl;
    cout << "mixed: "<< mixed << endl;
    std::stringstream ss;
    ss << "(" << lhs << "+" << mixed << ")";
    mixed = ss.str();
  }

  core::storage::version get_version() const {
    return core::storage::version();
  }
};

typedef core::framework::linear_mixable_helper<my_string, string> mixable_string;

TEST(linear_mixer, mix_order) {
  shared_ptr<linear_communication_stub> com(new linear_communication_stub);
  jubatus::util::concurrent::rw_mutex mutex;
  linear_mixer m(com, mutex, 1, 1);

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
  jubatus::util::concurrent::rw_mutex mutex;
  linear_mixer m(com, mutex, 1, 1);

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

