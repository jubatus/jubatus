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

#include "jubatus/util/lang/shared_ptr.h"

#include "../common/exception.hpp"
#include "../common/vector_util.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/revert.hpp"
#include "../recommender/recommender_factory.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using jubatus::core::framework::mixable_holder;
using jubatus::core::fv_converter::weight_manager;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

recommender::recommender(
    shared_ptr<core::recommender::recommender_base> recommender_method,
    shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixable_holder_(new mixable_holder),
      converter_(converter),
      recommender_(recommender_method) {
  recommender_->register_mixables_to_holder(*mixable_holder_);
  converter_->register_mixables_to_holder(*mixable_holder_);
}

recommender::~recommender() {
}

void recommender::clear_row(const std::string& id) {
  recommender_->clear_row(id);
}

void recommender::update_row(
    const std::string& id,
    const fv_converter::datum& dat) {
  core::recommender::sfv_diff_t v;
  converter_->convert_and_update_weight(dat, v);
  recommender_->update_row(id, v);
}

void recommender::clear() {
  recommender_->clear();
  converter_->clear_weights();
}

fv_converter::datum recommender::complete_row_from_id(const std::string& id) {
  common::sfv_t v;
  recommender_->complete_row(id, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

fv_converter::datum recommender::complete_row_from_datum(
    const fv_converter::datum& dat) {
  common::sfv_t u, v;
  converter_->convert(dat, u);
  recommender_->complete_row(u, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

std::vector<std::pair<std::string, float> > recommender::similar_row_from_id(
    const std::string& id,
    size_t ret_num) {
  std::vector<std::pair<std::string, float> > ret;
  recommender_->similar_row(id, ret, ret_num);
  return ret;
}

std::vector<std::pair<std::string, float> >
recommender::similar_row_from_datum(
    const fv_converter::datum& data,
    size_t size) {
  common::sfv_t v;
  converter_->convert(data, v);

  std::vector<std::pair<std::string, float> > ret;
  recommender_->similar_row(v, ret, size);
  return ret;
}

float recommender::calc_similality(
    const fv_converter::datum& l,
    const fv_converter::datum& r) {
  common::sfv_t v0, v1;
  converter_->convert(l, v0);
  converter_->convert(r, v1);
  return jubatus::core::recommender::recommender_base::calc_similality(v0, v1);
}

float recommender::calc_l2norm(const fv_converter::datum& q) {
  common::sfv_t v0;
  converter_->convert(q, v0);
  return jubatus::core::recommender::recommender_base::calc_l2norm(v0);
}

fv_converter::datum recommender::decode_row(const std::string& id) {
  common::sfv_t v;
  recommender_->decode_row(id, v);

  fv_converter::datum ret;
  fv_converter::revert_feature(v, ret);
  return ret;
}

std::vector<std::string> recommender::get_all_rows() {
  std::vector<std::string> ret;
  recommender_->get_all_row_ids(ret);
  return ret;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
