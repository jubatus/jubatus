#include <gtest/gtest.h>

#include "jubatus/core/framework/mixable.hpp"
#include "linear_mixer.hpp"

#include <iostream>

#include "../../../core/framework/mixable.hpp"
#include "../../../core/common/byte_buffer.hpp"

using namespace std;
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

}

class linear_communication_stub : public linear_communication {
 public:
  size_t update_members() { return 4; }

  pfi::lang::shared_ptr<common::try_lockable> create_lock() {}

  void get_diff(common::mprpc::rpc_result_object& result) const {
    result.response.push_back(make_response("1"));
    result.response.push_back(make_response("2"));
    result.response.push_back(make_response("3"));
    result.response.push_back(make_response("4"));
  }

  void put_diff(const vector<byte_buffer>& mixed) const {
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

  const vector<string>& get_mixed() const {
    return mixed_;
  }

 private:
  mutable vector<string> mixed_;
};

// <<<<<<< HEAD
// struct mixable_string : public mixable0 {
// =======
struct mixable_string
    : public core::framework::mixable<mixable_string, core::common::byte_buffer> {
// >>>>>>> develop
 public:
  core::common::byte_buffer get_diff_impl() const { return core::common::byte_buffer(); }
  void put_diff_impl(const byte_buffer&) {}
  void mix_impl(const byte_buffer& lhs, const byte_buffer& rhs, byte_buffer& mixed) const {
    stringstream ss;
    ss << "(" << string(lhs.ptr(), lhs.size()) << "+" << string(rhs.ptr(), rhs.size()) << ")";
    string s = ss.str();
    mixed.assign(s.data(), s.size());
  }
  string get_pull_argument() const { return string(); }
  string pull(const string&) const { return string(); }
  void push(const string&) {}
  void save(ostream&) {}
  void load(istream&) {}
  void clear() {}
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
