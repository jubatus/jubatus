// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "local_storage.hpp"
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include "jubatus/util/data/intern.h"
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/serialization/unordered_map.h"

using std::string;

namespace jubatus {
namespace core {
namespace storage {

local_storage::local_storage() {
}

local_storage::~local_storage() {
}

void local_storage::get(const string& feature, feature_val1_t& ret) const {
  ret.clear();
  id_features3_t::const_iterator cit = tbl_.find(feature);
  if (cit == tbl_.end()) {
    return;
  }
  const id_feature_val3_t& m = cit->second;
  for (id_feature_val3_t::const_iterator it = m.begin(); it != m.end(); ++it) {
    ret.push_back(make_pair(class2id_.get_key(it->first), it->second.v1));
  }
}

void local_storage::get2(const string& feature, feature_val2_t& ret) const {
  ret.clear();
  id_features3_t::const_iterator cit = tbl_.find(feature);
  if (cit == tbl_.end()) {
    return;
  }
  const id_feature_val3_t& m = cit->second;
  for (id_feature_val3_t::const_iterator it = m.begin(); it != m.end(); ++it) {
    ret.push_back(make_pair(class2id_.get_key(it->first),
                            val2_t(it->second.v1, it->second.v2)));
  }
}

void local_storage::get3(const string& feature, feature_val3_t& ret) const {
  ret.clear();
  id_features3_t::const_iterator cit = tbl_.find(feature);
  if (cit == tbl_.end()) {
    return;
  }
  const id_feature_val3_t& m = cit->second;
  for (id_feature_val3_t::const_iterator it = m.begin(); it != m.end(); ++it) {
    ret.push_back(make_pair(class2id_.get_key(it->first), it->second));
  }
}

void local_storage::inp(const common::sfv_t& sfv, map_feature_val1_t& ret)
    const {
  ret.clear();

  std::vector<float> ret_id(class2id_.size());
  for (common::sfv_t::const_iterator it = sfv.begin(); it != sfv.end(); ++it) {
    const string& feature = it->first;
    const float val = it->second;
    id_features3_t::const_iterator it2 = tbl_.find(feature);
    if (it2 == tbl_.end()) {
      continue;
    }
    const id_feature_val3_t& m = it2->second;
    for (id_feature_val3_t::const_iterator it3 = m.begin(); it3 != m.end();
        ++it3) {
      ret_id[it3->first] += it3->second.v1 * val;
    }
  }

  for (size_t i = 0; i < ret_id.size(); ++i) {
    if (ret_id[i] == 0.f) {
      continue;
    }
    ret[class2id_.get_key(i)] = ret_id[i];
  }
}

void local_storage::set(
    const string& feature,
    const string& klass,
    const val1_t& w) {
  tbl_[feature][class2id_.get_id(klass)].v1 = w;
}

void local_storage::set2(
    const string& feature,
    const string& klass,
    const val2_t& w) {
  val3_t& val3 = tbl_[feature][class2id_.get_id(klass)];
  val3.v1 = w.v1;
  val3.v2 = w.v2;
}

void local_storage::set3(
    const string& feature,
    const string& klass,
    const val3_t& w) {
  tbl_[feature][class2id_.get_id(klass)] = w;
}

void local_storage::get_status(std::map<string, std::string>& status) const {
  status["num_features"] =
    jubatus::util::lang::lexical_cast<std::string>(tbl_.size());
  status["num_classes"] =
    jubatus::util::lang::lexical_cast<std::string>(class2id_.size());
}

float feature_fabssum(const id_feature_val3_t& f) {
  float sum = 0.f;
  for (id_feature_val3_t::const_iterator it = f.begin(); it != f.end(); ++it) {
    sum += fabs(it->second.v1);
  }
  return sum;
}

void local_storage::bulk_update(
    const common::sfv_t& sfv,
    float step_width,
    const string& inc_class,
    const string& dec_class) {
  uint64_t inc_id = class2id_.get_id(inc_class);
  typedef common::sfv_t::const_iterator iter_t;
  if (dec_class != "") {
    uint64_t dec_id = class2id_.get_id(dec_class);
    for (iter_t it = sfv.begin(); it != sfv.end(); ++it) {
      float val = it->second * step_width;
      id_feature_val3_t& feature_row = tbl_[it->first];
      feature_row[inc_id].v1 += val;
      feature_row[dec_id].v1 -= val;
    }
  } else {
    for (iter_t it = sfv.begin(); it != sfv.end(); ++it) {
      float val = it->second * step_width;
      id_feature_val3_t& feature_row = tbl_[it->first];
      feature_row[inc_id].v1 += val;
    }
  }
}

void local_storage::update(
    const string& feature,
    const string& inc_class,
    const string& dec_class,
    const val1_t& v) {
  id_feature_val3_t& feature_row = tbl_[feature];
  feature_row[class2id_.get_id(inc_class)].v1 += v;
  feature_row[class2id_.get_id(dec_class)].v1 -= v;
}

void local_storage::clear() {
  // Clear and minimize
  id_features3_t().swap(tbl_);
  common::key_manager().swap(class2id_);
}

void local_storage::pack(msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack(*this);
}

void local_storage::unpack(msgpack::object o) {
  o.convert(this);
}

std::string local_storage::type() const {
  return "local_storage";
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
