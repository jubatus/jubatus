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

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include "recommender_base.hpp"
#include "../common/vector_util.hpp"

using std::make_pair;
using std::pair;
using std::string;
using std::sort;
using std::vector;

namespace jubatus {
namespace core {
namespace recommender {

const uint64_t recommender_base::complete_row_similar_num_ = 128;

recommender_base::recommender_base() {
}

recommender_base::~recommender_base() {
}

void recommender_base::similar_row(
    const std::string& id, std::vector<std::pair<std::string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  sfv_t sfv;
  orig_.get_row(id, sfv);
  similar_row(sfv, ids, ret_num);
}

void recommender_base::neighbor_row(
    const string& id,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  sfv_t sfv;
  orig_.get_row(id, sfv);
  neighbor_row(sfv, ids, ret_num);
}

void recommender_base::decode_row(const std::string& id, sfv_t& ret) const {
  ret.clear();
  orig_.get_row(id, ret);
}

void recommender_base::complete_row(const std::string& id, sfv_t& ret) const {
  ret.clear();
  sfv_t sfv;
  orig_.get_row(id, sfv);
  complete_row(sfv, ret);
}

void recommender_base::complete_row(const sfv_t& query, sfv_t& ret) const {
  ret.clear();
  vector<pair<string, float> > ids;
  similar_row(query, ids, complete_row_similar_num_);
  if (ids.size() == 0) {
    return;
  }

  size_t exist_row_num = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    sfv_t row;
    orig_.get_row(ids[i].first, row);
    if (row.size() == 0) {
      continue;
    } else {
      ++exist_row_num;
    }
    float ratio = ids[i].second;
    for (size_t j = 0; j < row.size(); ++j) {
      ret.push_back(make_pair(row[j].first, row[j].second * ratio));
    }
  }

  if (exist_row_num == 0) {
    return;
  }
  sort_and_merge(ret);
  for (size_t i = 0; i < ret.size(); ++i) {
    ret[i].second /= exist_row_num;
  }
}

void recommender_base::save(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << orig_;
  save_impl(os);
}
void recommender_base::load(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> orig_;
  load_impl(is);
}

float recommender_base::calc_similality(sfv_t& q1, sfv_t& q2) {
  float q1_norm = calc_l2norm(q1);
  float q2_norm = calc_l2norm(q2);
  if (q1_norm == 0.f || q2_norm == 0.f) {
    return 0.f;
  }
  sort(q1.begin(), q1.end());
  sort(q2.begin(), q2.end());

  size_t i1 = 0;
  size_t i2 = 0;
  float ret = 0.f;
  while (i1 < q1.size() && i2 < q2.size()) {
    const string& ind1 = q1[i1].first;
    const string& ind2 = q2[i2].first;
    if (ind1 < ind2) {
      ++i1;
    } else if (ind1 > ind2) {
      ++i2;
    } else {
      ret += q1[i1].second * q2[i2].second;
      ++i1;
      ++i2;
    }
  }

  return ret / q1_norm / q2_norm;
}

float recommender_base::calc_l2norm(const sfv_t& query) {
  float ret = 0.f;
  for (size_t i = 0; i < query.size(); ++i) {
    ret += query[i].second * query[i].second;
  }
  return sqrt(ret);
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
