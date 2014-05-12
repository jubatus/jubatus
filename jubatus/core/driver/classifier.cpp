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

#include "classifier.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../classifier/classifier_factory.hpp"
#include "../common/vector_util.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::fv_converter::weight_manager;

namespace jubatus {
namespace core {
namespace driver {

classifier::classifier(
    shared_ptr<core::classifier::classifier_base> classifier_method,
    shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : converter_(converter)
    , classifier_(classifier_method)
    , mixable_classifier_model_(classifier_method->get_storage())
    , wm_(core::fv_converter::mixable_weight_manager::model_ptr(new weight_manager)) {

  register_mixable(&mixable_classifier_model_);
  register_mixable(&wm_);

  converter_->set_weight_manager(wm_.get_model());
}

classifier::~classifier() {
}

void classifier::train(const pair<string, fv_converter::datum>& data) {
  common::sfv_t v;
  converter_->convert_and_update_weight(data.second, v);
  common::sort_and_merge(v);
  classifier_->train(v, data.first);
}

jubatus::core::classifier::classify_result classifier::classify(
    const fv_converter::datum& data) const {
  common::sfv_t v;
  converter_->convert(data, v);

  jubatus::core::classifier::classify_result scores;
  classifier_->classify_with_scores(v, scores);
  return scores;
}

void classifier::get_status(std::map<string, string>& status) const {
  classifier_->get_status(status);
}

void classifier::clear() {
  classifier_->clear();
  converter_->clear_weights();
}

std::vector<std::string> classifier::get_labels() const {
  return classifier_->get_labels();
}
bool classifier::set_label(const std::string& label) {
  return classifier_->set_label(label);
}

void classifier::pack(framework::packer& pk) const {
  pk.pack_array(2);
  classifier_->get_storage()->pack(pk);
  wm_.get_model()->pack(pk);
}

void classifier::unpack(msgpack::object o) {
  if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
    throw msgpack::type_error();
  }

  // clear before load
  classifier_->clear();
  converter_->clear_weights();
  classifier_->get_storage()->unpack(o.via.array.ptr[0]);
  wm_.get_model()->unpack(o.via.array.ptr[1]);
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
