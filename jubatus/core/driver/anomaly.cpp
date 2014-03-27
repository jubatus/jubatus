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

#include "anomaly.hpp"

#include <string>
#include <utility>
#include <vector>

#include "../anomaly/anomaly_factory.hpp"
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
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

anomaly::anomaly(
    shared_ptr<jubatus::core::anomaly::anomaly_base> anomaly_method,
    shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixable_holder_(new mixable_holder),
      converter_(converter),
      anomaly_(anomaly_method) {
  anomaly_->register_mixables_to_holder(*mixable_holder_);
  converter_->register_mixables_to_holder(*mixable_holder_);
}

anomaly::~anomaly() {
}

void anomaly::clear_row(const std::string& id) {
  anomaly_->clear_row(id);
}

pair<string, float> anomaly::add(
    const string& id,
    const fv_converter::datum& d) {
  float score;
  if (anomaly_->is_updatable()) {
    score = this->update(id, d);
  } else {
    score = this->overwrite(id, d);
  }
  return make_pair(id, score);
}

float anomaly::update(const string& id, const fv_converter::datum& d) {
  common::sfv_t v;
  converter_->convert_and_update_weight(d, v);

  anomaly_->update_row(id, v);
  return anomaly_->calc_anomaly_score(id);
}

float anomaly::overwrite(const string& id, const fv_converter::datum& d) {
  common::sfv_t v;
  converter_->convert_and_update_weight(d, v);

  anomaly_->set_row(id, v);
  return anomaly_->calc_anomaly_score(id);
}

void anomaly::clear() {
  anomaly_->clear();
  converter_->clear_weights();
}

float anomaly::calc_score(const fv_converter::datum& d) const {
  common::sfv_t v;
  converter_->convert(d, v);
  return anomaly_->calc_anomaly_score(v);
}

vector<string> anomaly::get_all_rows() const {
  vector<string> ids;
  anomaly_->get_all_row_ids(ids);
  return ids;
}

uint64_t anomaly::find_max_int_id() const {
  return anomaly_->find_max_int_id();
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
