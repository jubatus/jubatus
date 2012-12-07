#include <gtest/gtest.h>
#include "linear_mixer.hpp"

#include <iostream>
#include <cstdio>

#include "../mixable.hpp"

using namespace std;
using pfi::lang::shared_ptr;
using jubatus::common::mprpc::byte_buffer;

namespace jubatus {
namespace framework {
namespace mixer {

namespace {

jubatus::common::mprpc::rpc_response_t make_response(const string& s) {
  jubatus::common::mprpc::rpc_response_t res;

  res.zone = shared_ptr<msgpack::zone>(new msgpack::zone);
  // array of `diff`s
  res.response.a3 = msgpack::object(vector<string>(1, s), res.zone.get());

#if 0 // for debug
  msgpack::object o = res.response.a3;
  cout << o.type << " " << o.via.array.size << " " <<o  << endl;
#endif
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

  void put_diff(const byte_buffer& mixed) const {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, mixed.ptr(), mixed.size());
    msgpack::object o = msg.get();
    o.convert(&mixed_);
  }

  const vector<string> get_mixed() const {
    vector<string> mixed;
    for (vector<byte_buffer>::const_iterator it = mixed_.begin(); it != mixed_.end(); ++it) {
      mixed.push_back(string(it->ptr(), it->size()));
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

  mixable_string s;
  pfi::lang::shared_ptr<mixable_holder> holder(new mixable_holder(diff_model_bundler::create(s)));
  m.set_mixable_holder(holder);

  m.mix();

  vector<string> mixed = com->get_mixed();
  ASSERT_EQ(1u, mixed.size());
  EXPECT_EQ("(4+(3+(2+1)))", mixed[0]);
}

}
}
}
