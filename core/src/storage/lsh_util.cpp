// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "lsh_util.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

using std::copy;
using std::greater;
using std::make_pair;
using std::pair;
using std::sort;
using std::vector;
using std::priority_queue;

namespace jubatus {
namespace core {
namespace storage {

namespace {

typedef pair<float, pair<int, vector<int> > > diff_type;
typedef priority_queue<
    diff_type,
    vector<diff_type>,
    greater<diff_type> > heap_type;

void partition(
    const vector<float>& hash,
    size_t num_hash_tables,
    vector<vector<float> >& hashes) {
  const size_t hash_size = hash.size() / num_hash_tables;
  hashes.resize(num_hash_tables);
  for (size_t i = 0; i < num_hash_tables; ++i) {
    hashes[i].resize(hash_size);
    copy(&hash[i * hash_size], &hash[(i + 1) * hash_size], &hashes[i][0]);
  }
}

void threshold(const vector<float>& hash, lsh_vector& lv) {
  lv.resize_and_clear(hash.size());
  for (size_t j = 0; j < hash.size(); ++j) {
    lv.set(j, floor(hash[j]));
  }
}

lsh_vector perturbe(
    const lsh_vector& src,
    const vector<int>& diff,
    const vector<pair<float, int> >& cands) {
  lsh_vector ret(src);
  for (size_t i = 0; i < diff.size(); ++i) {
    const int d_idx = cands[diff[i]].second;
    if (d_idx >= 0) {
      ret.set(d_idx, ret.get(d_idx) + 1);
    } else {
      ret.set(~d_idx, ret.get(~d_idx) - 1);
    }
  }
  return ret;
}

}  // namespace

lsh_probe_generator::lsh_probe_generator(
    const vector<float>& hash,
    size_t num_hash_tables) {
  partition(hash, num_hash_tables, hash_);
  base_.resize(hash_.size());
  for (size_t i = 0; i < hash_.size(); ++i) {
    threshold(hash_[i], base_[i]);
  }
}

void lsh_probe_generator::init() {
  const int base_size = base_[0].size();

  perturbation_sets_.resize(base_.size());
  for (size_t i = 0; i < base_.size(); ++i) {
    vector<pair<float, int> >& cands = perturbation_sets_[i];
    cands.resize(2 * base_size);
    for (int j = 0; j < base_size; ++j) {
      const float dist = hash_[i][j] - base_[i].get(j);
      cands[j * 2] = make_pair((1 - dist) * (1 - dist), j);
      cands[j * 2 + 1] = make_pair(dist * dist, ~j);
    }
    sort(cands.begin(), cands.end());
  }

  for (size_t i = 0; i < base_.size(); ++i) {
    if (!perturbation_sets_[i].empty()) {
      heap_.push(
          make_pair(perturbation_sets_[i][0].first,
                    make_pair(i, vector<int>(1))));
    }
  }
}

lsh_vector lsh_probe_generator::base_all() const {
  const size_t base_size = base_[0].size();
  lsh_vector lv(base_size * base_.size());
  for (size_t i = 0; i < base_.size(); ++i) {
    for (size_t j = 0; j < base_size; ++j) {
      lv.set(i * base_size + j, base_[i].get(j));
    }
  }
  return lv;
}

pair<size_t, lsh_vector> lsh_probe_generator::get_next_table_and_vector() {
  if (heap_.empty()) {
    return make_pair(-1ul, lsh_vector());
  }
  const size_t i = heap_.top().second.first;
  pair<size_t, lsh_vector> ret(
      i,
      perturbe(base_[i], heap_.top().second.second, perturbation_sets_[i]));
  next_perturbations();
  return ret;
}

void lsh_probe_generator::next_perturbations() {
  const int index = heap_.top().second.first;
  const vector<pair<float, int> >& cands = perturbation_sets_[index];

  vector<int> shifted = heap_.top().second.second;
  ++shifted.back();

  vector<int> expanded = heap_.top().second.second;
  expanded.push_back(expanded.back() + 1);

  const float score_base = heap_.top().first;
  heap_.pop();

  if ((size_t) shifted.back() < cands.size()) {
    const float score_diff = cands[shifted.back()].first
        - cands[shifted.back() - 1].first;
    heap_.push(make_pair(score_base + score_diff, make_pair(index, shifted)));
  }

  if ((size_t) expanded.back() < cands.size()) {
    const float score_diff = cands[expanded.back()].first;
    heap_.push(make_pair(score_base + score_diff, make_pair(index, expanded)));
  }
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
