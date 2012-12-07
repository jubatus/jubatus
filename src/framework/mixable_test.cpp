#include <gtest/gtest.h>

#include "mixable.hpp"

#include <sstream>

using namespace std;
using jubatus::common::mprpc::byte_buffer;

namespace jubatus {
namespace framework {

struct int_model {
  int_model() : value(0) {}

  int value;

  void save(std::ostream & ofs) {
    ofs << value;
  }

  void load(std::istream & ifs) {
    ifs >> value;
  }
};

class mixable_int : public mixable<int_model, int> {
 public:
  mixable_int() : diff_() {}

  void clear() {}

  int get_diff_impl() const {
    return diff_;
  }

  void put_diff_impl(const int& n) {
    get_model()->value += n;
    diff_ = 0;
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

//TEST(mixable, config_not_set) {
//  mixable_int m;
//  EXPECT_THROW(m.get_diff_impl(), config_not_set);
//  EXPECT_THROW(m.put_diff(byte_buffer()), config_not_set);
//}

TEST(mixable, save_load) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));
  m.get_model()->value = 10;

  stringstream ss;
  m.save(ss);
  m.get_model()->value = 5;
  m.load(ss);
  EXPECT_EQ(10, m.get_model()->value);
}

TEST(mixable, trivial) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));
  pfi::lang::shared_ptr<model_bundler> bundler(diff_model_bundler::create(m));

  diff_model_bundler* b = dynamic_cast<diff_model_bundler*>(bundler.get());

  m.add(10);

  byte_buffer diff1 = b->get_diff();
  byte_buffer diff2 = b->get_diff();

  byte_buffer mixed;
  b->mix(diff1, diff2, mixed);

  b->put_diff(mixed);

  EXPECT_EQ(20, m.get_model()->value);
}

}
}
