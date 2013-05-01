// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_DRIVER_RECOMMENDER_HPP_
#define JUBATUS_CORE_DRIVER_RECOMMENDER_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include "../recommender/recommender_base.hpp"
#include "../framework/mixable.hpp"
#include "diffv.hpp"
#include "linear_function_mixer.hpp"
#include "mixable_weight_manager.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"

namespace jubatus {
namespace core {
namespace driver {

struct mixable_recommender : public framework::mixable<
    jubatus::core::recommender::recommender_base,
    std::string> {
  std::string get_diff_impl() const {
    std::string ret;
    get_model()->get_const_storage()->get_diff(ret);
    return ret;
  }

  void put_diff_impl(const std::string& v) {
    get_model()->get_storage()->set_mixed_and_clear_diff(v);
  }

  void mix_impl(
      const std::string& lhs,
      const std::string& rhs,
      std::string& mixed) const {
    mixed = lhs;
    get_model()->get_const_storage()->mix(rhs, mixed);
  }

  void clear() {
  }
};

class recommender {
 public:
  recommender(
      jubatus::core::recommender::recommender_base* recommender_method,
      pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter);
  virtual ~recommender();

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

#if 0
  storage::storage_base* get_model() const {
    return recommender_.get_model().get();
  }
#endif

  void clear_row(const std::string& id);
  void update_row(const std::string& id, const fv_converter::datum& dat);
  void clear();

  fv_converter::datum complete_row_from_id(const std::string& id);
  fv_converter::datum complete_row_from_datum(const fv_converter::datum& dat);
  std::vector<std::pair<std::string, float> > similar_row_from_id(
      const std::string& id,
      size_t ret_num);
  std::vector<std::pair<std::string, float> > similar_row_from_datum(
      const fv_converter::datum& data,
      size_t size);

  float calc_similality(
      const fv_converter::datum& l,
      const fv_converter::datum& r);
  float calc_l2norm(const fv_converter::datum& q);

  fv_converter::datum decode_row(const std::string& id);
  std::vector<std::string> get_all_rows();

 private:
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  mixable_recommender recommender_;
  mixable_weight_manager wm_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_RECOMMENDER_HPP_
