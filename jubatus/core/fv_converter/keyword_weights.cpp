// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <cmath>
#include <string>
#include <utility>
#include "../common/type.hpp"
#include "datum_to_fv_converter.hpp"
#include "keyword_weights.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

struct is_zero {
  bool operator()(const std::pair<std::string, float>& p) {
    return p.second == 0;
  }
};

}  // namespace

keyword_weights::keyword_weights()
    : document_count_(),
      document_frequencies_(),
      weights_() {
}

void keyword_weights::update_document_frequency(const sfv_t& fv) {
  ++document_count_;
  for (sfv_t::const_iterator it = fv.begin(); it != fv.end(); ++it) {
    ++document_frequencies_[it->first];
  }
}

void keyword_weights::add_weight(const std::string& key, float weight) {
  weights_[key] = weight;
}

float keyword_weights::get_user_weight(const std::string& key) const {
  weight_t::const_iterator wit = weights_.find(key);
  if (wit != weights_.end()) {
    return wit->second;
  } else {
    return 0;
  }
}

void keyword_weights::merge(const keyword_weights& w) {
  document_count_ += w.document_count_;
  document_frequencies_.add(w.document_frequencies_);
  weight_t weights(w.weights_);
  weights.insert(weights_.begin(), weights_.end());
  weights_.swap(weights);
}

void keyword_weights::clear() {
  document_count_ = 0;
  document_frequencies_.clear();
  weight_t().swap(weights_);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
