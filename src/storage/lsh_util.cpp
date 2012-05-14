// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include <queue>
#include <functional>
#include <utility>

using namespace std;

namespace jubatus {
namespace storage {

namespace {

typedef pair<float, vector<int> > scored_diff_type;
typedef priority_queue<scored_diff_type,
                       vector<scored_diff_type>,
                       greater<scored_diff_type> > pq_type;

lsh_vector threshold(const vector<float>& hash) {
  lsh_vector lv(hash.size());
  for (size_t j = 0; j < hash.size(); ++j) {
    lv.set(j, floor(hash[j]));
  }
  return lv;
}

lsh_vector perturbe(const lsh_vector& src,
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

void next_perturbations(const vector<pair<float, int> >& cands,
                        pq_type& pq) {
  vector<int> shifted = pq.top().second;
  ++shifted.back();

  vector<int> expanded = pq.top().second;
  expanded.push_back(expanded.back() + 1);

  const float score_base = pq.top().first;
  pq.pop();

  if ((size_t)shifted.back() < cands.size()) {
    const float score_diff =
        cands[shifted.back()].first - cands[shifted.back() - 1].first;
    pq.push(make_pair(score_base + score_diff, shifted));
  }

  if ((size_t) expanded.back() < cands.size()) {
    const float score_diff = cands[expanded.back()].first;
    pq.push(make_pair(score_base + score_diff, expanded));
  }
}

}

void generate_lsh_probe(const vector<float>& hash,
                        size_t num_probe,
                        lsh_vector& key,
                        vector<lsh_vector>& probe) {
  key = threshold(hash);

  vector<pair<float, int> > cands(2 * hash.size());
  for (int i = 0; (size_t)i < hash.size(); ++i) {
    const float dist = hash[i] - key.get(i);
    cands[i * 2] = make_pair((1 - dist) * (1 - dist), i);
    cands[i * 2 + 1] = make_pair(dist * dist, ~i);
  }
  sort(cands.begin(), cands.end());

  pq_type pq;
  if (!cands.empty()) {
    pq.push(make_pair(cands[0].first, vector<int>(1)));
  }

  probe.clear();
  probe.push_back(key);
  for (size_t i = 0; i < num_probe; ++i) {
    if (pq.empty()) {
      break;
    }
    probe.push_back(perturbe(key, pq.top().second, cands));
    next_perturbations(cands, pq);
  }
}

}
}
