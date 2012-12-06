#include <gtest/gtest.h>
#include "linear_mixer.hpp"

#include <iostream>

#include "../mixable.hpp"

using namespace std;
using pfi::lang::shared_ptr;
using jubatus::common::mprpc::byte_buffer;

namespace jubatus {
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

jubatus::common::mprpc::rpc_response_t make_response(const string& s) {
  jubatus::common::mprpc::rpc_response_t res;

  res.zone = shared_ptr<msgpack::zone>(new msgpack::zone);
  res.response.a3 = msgpack::object(make_packed_vector(s), res.zone.get());
  return res;
}

}

class linear_communication_stub : public linear_communication {
 public:
  size_t update_members() { return 4; }

  pfi::lang::shared_ptr<common::try_lockable> create_lock() { return pfi::lang::shared_ptr<common::try_lockable>(); }

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
    for (vector<byte_buffer>::const_iterator it = mixed_.begin(); it != mixed_.end(); ++it) {
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

struct mixable_string : public mixable<mixable_string, string> {
 public:
  string get_diff_impl() const { return string(); }
  void put_diff_impl(const string&) {}
  void mix_impl(const string& lhs, const string& rhs, string& mixed) const {
    stringstream ss;
    ss << "(" << lhs << "+" << rhs << ")";
    mixed = ss.str();
  }
  void save(ostream&) {}
  void load(istream&) {}
  void clear() {}
};

TEST(linear_mixer, mix_order) {
  shared_ptr<linear_communication_stub> com(new linear_communication_stub);
  linear_mixer m(com, 1, 1);

  pfi::lang::shared_ptr<mixable_holder> holder(new mixable_holder());
  m.set_mixable_holder(holder);

  mixable_string s;
  holder->register_mixable(&s);

  m.mix();

  vector<string> mixed = com->get_mixed();
  ASSERT_EQ(1u, mixed.size());
  EXPECT_EQ("(4+(3+(2+1)))", mixed[0]);
}

}
}
}
