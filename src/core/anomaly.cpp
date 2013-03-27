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
#include "../common/global_id_generator_base.hpp"
#include "../common/util.hpp"
#include "../common/vector_util.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/revert.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using jubatus::framework::convert;
using jubatus::framework::mixer::create_mixer;
using jubatus::framework::mixable_holder;
using jubatus::fv_converter::weight_manager;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {

anomaly::anomaly(
    jubatus::anomaly::anomaly_base* anomaly_method,
    pfi::lang::shared_ptr<framework::mixer::mixer> mixer,
    pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixer_(mixer),
      mixable_holder_(new mixable_holder),
      converter_(converter) {
  common::cshared_ptr<jubatus::anomaly::anomaly_base>
      anomaly_method_p(anomaly_method);
  anomaly_.set_model(anomaly_method_p);
  wm_.set_model(mixable_weight_manager::model_ptr(new weight_manager));

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&anomaly_);
  mixable_holder_->register_mixable(&wm_);

#if 0
  // TODO(oda): original anomaly_serv doesn't call set_weigt_manager()
  // Is this right?
  (*converter_).set_weight_manager(wm_.get_model());
#endif
}

anomaly::~anomaly() {
}

void anomaly::clear_row(const std::string& id) {
  anomaly_.get_model()->clear_row(id);
}

pair<string, float> anomaly::add(
    const pfi::lang::shared_ptr<common::global_id_generator_base>& idgen,
    const fv_converter::datum& d) {
  uint64_t id = idgen->generate();
  string id_str = pfi::lang::lexical_cast<string>(id);

  float score = this->update(id_str, d);
  return make_pair(id_str, score);
}

float anomaly::update(const string& id, const fv_converter::datum& d) {
  sfv_t v;
  converter_->convert_and_update_weight(d, v);

  anomaly_.get_model()->update_row(id, v);
  return anomaly_.get_model()->calc_anomaly_score(id);
}

void anomaly::clear() {
  anomaly_.get_model()->clear();
}

float anomaly::calc_score(const fv_converter::datum& d) const {
  sfv_t v;
  converter_->convert(d, v);
  return anomaly_.get_model()->calc_anomaly_score(v);
}

vector<string> anomaly::get_all_rows() const {
  vector<string> ids;
  anomaly_.get_model()->get_all_row_ids(ids);
  return ids;
}

}  // namespace core
}  // namespace jubatus
