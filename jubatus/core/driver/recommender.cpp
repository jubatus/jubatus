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

#include "recommender.hpp"

#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/shared_ptr.h>

#include "../recommender/recommender_factory.hpp"
#include "../common/vector_util.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/revert.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using jubatus::core::framework::mixable_holder;
using jubatus::core::fv_converter::weight_manager;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

recommender::recommender(
    jubatus::core::recommender::recommender_base* recommender_method,
    pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixable_holder_(new mixable_holder),
      converter_(converter) {
  pfi::lang::shared_ptr<jubatus::core::recommender::recommender_base>
      recommender_method_p(recommender_method);
  recommender_.set_model(recommender_method_p);
  wm_.set_model(mixable_weight_manager::model_ptr(new weight_manager));

  mixable_holder_->register_mixable(&recommender_);
  mixable_holder_->register_mixable(&wm_);

  (*converter_).set_weight_manager(wm_.get_model());
}

recommender::~recommender() {
}

void recommender::clear_row(const std::string& id) {
  recommender_.get_model()->clear_row(id);
}

void recommender::update_row(
    const std::string& id,
    const fv_converter::datum& dat) {
  sfv_diff_t v;
  converter_->convert_and_update_weight(dat, v);
  recommender_.get_model()->update_row(id, v);
}

void recommender::clear() {
  recommender_.get_model()->clear();
  wm_.clear();
}

fv_converter::datum recommender::complete_row_from_id(const std::string& id) {
  sfv_t v;
  recommender_.get_model()->complete_row(id, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

fv_converter::datum recommender::complete_row_from_datum(
    const fv_converter::datum& dat) {
  sfv_t u, v;
  converter_->convert(dat, u);
  recommender_.get_model()->complete_row(u, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

std::vector<std::pair<std::string, float> > recommender::similar_row_from_id(
    const std::string& id,
    size_t ret_num) {
  std::vector<std::pair<std::string, float> > ret;
  recommender_.get_model()->similar_row(id, ret, ret_num);
  return ret;
}

std::vector<std::pair<std::string, float> >
recommender::similar_row_from_datum(
    const fv_converter::datum& data,
    size_t size) {
  sfv_t v;
  converter_->convert(data, v);

  std::vector<std::pair<std::string, float> > ret;
  recommender_.get_model()->similar_row(v, ret, size);
  return ret;
}

float recommender::calc_similality(
    const fv_converter::datum& l,
    const fv_converter::datum& r) {
  sfv_t v0, v1;
  converter_->convert(l, v0);
  converter_->convert(r, v1);
  return jubatus::core::recommender::recommender_base::calc_similality(v0, v1);
}

float recommender::calc_l2norm(const fv_converter::datum& q) {
  sfv_t v0;
  converter_->convert(q, v0);
  return jubatus::core::recommender::recommender_base::calc_l2norm(v0);
}

fv_converter::datum recommender::decode_row(const std::string& id) {
  sfv_t v;
  recommender_.get_model()->decode_row(id, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

std::vector<std::string> recommender::get_all_rows() {
  std::vector<std::string> ret;
  recommender_.get_model()->get_all_row_ids(ret);
  return ret;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
