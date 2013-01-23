#include <pficommon/lang/bind.h>
#include "linear_function_mixer.hpp"

using namespace std;
using jubatus::storage::val3_t;
using jubatus::storage::feature_val3_t;
using jubatus::storage::features3_t;
using namespace pfi::lang;

namespace jubatus {
namespace server {

namespace {

val3_t mix_val3(double w1, double w2, const val3_t& lhs, const val3_t& rhs) {
  return val3_t((w1 * lhs.v1 + w2 * rhs.v1) / (w1 + w2),
                std::min(lhs.v2, rhs.v2),
                (w1 * lhs.v3 + w2 * rhs.v3) / (w1 + w2));
}

feature_val3_t mix_feature(double w1, double w2, const feature_val3_t& lhs,
                           const feature_val3_t& rhs) {
  val3_t def(0, 1, 0);
  feature_val3_t ret(lhs);
  storage::detail::binop(ret, rhs, bind(mix_val3, w1, w2, _1, _2), def);
  return ret;
}

}

void linear_function_mixer::mix_impl(const diffv& lhs, const diffv& rhs,
                                     diffv& mixed) const {
  features3_t l(lhs.v);
  const features3_t& r(rhs.v);
  storage::detail::binop(l, r, bind(mix_feature, lhs.count, rhs.count, _1, _2));
  mixed.v.swap(l);
  mixed.count = lhs.count + rhs.count;
}

diffv linear_function_mixer::get_diff_impl() const {
  diffv ret;
  ret.count = 1;  //FIXME mixer_->get_count();
  get_model()->get_diff(ret.v);
  return ret;
}

void linear_function_mixer::put_diff_impl(const diffv& v) {
  get_model()->set_average_and_clear_diff(v.v);
}

void linear_function_mixer::clear() {
}

}  // namespace server
}  // namespace jubatus
