#include "mixable_weight_manager.hpp"

#include <pficommon/data/serialization.h>

#include "../fv_converter/weight_manager.hpp"

using jubatus::fv_converter::keyword_weights;
using jubatus::fv_converter::weight_manager;

namespace jubatus {
namespace server {

keyword_weights mixable_weight_manager::get_diff_impl() const {
  return get_model()->get_diff();
}

void mixable_weight_manager::put_diff_impl(const fv_converter::keyword_weights& diff) {
  get_model()->put_diff(diff);
}

void mixable_weight_manager::mix_impl(const keyword_weights& lhs,
                                      const keyword_weights& rhs,
                                      keyword_weights& acc) const {
  acc = rhs;
  acc.merge(lhs);
}

void mixable_weight_manager::clear() {
}

}
}
