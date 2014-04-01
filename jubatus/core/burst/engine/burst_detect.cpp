// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "burst_detect.hpp"

#include <math.h>  // for ::lgamma (std::lgamma is not provided in C++03)
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <iostream>

namespace jubatus {
namespace core {
namespace burst {
namespace engine {

// #define BURST_DEBUG

namespace {

// In enumerate burst detect, automaton has the following 2 states.
const int kStatesNum = 2;
// - base state
const int kBaseState = 0;
// - burst state
const int kBurstState = 1;

const double kDefaultBatchWeight = -1;

std::vector<double> get_p_vector(
    const std::vector<uint32_t>& d_vector,
    const std::vector<uint32_t>& r_vector,
    double scaling_param) {
  std::vector<double> ret(kStatesNum);
  // D := \sum d
  const int32_t D = std::accumulate(d_vector.begin(), d_vector.end(), 0);
  // R := \sum r
  const int32_t R = std::accumulate(r_vector.begin(), r_vector.end(), 0);
  ret[kBaseState] = static_cast<double>(R) / static_cast<double>(D);
  ret[kBurstState] = scaling_param * ret[kBaseState];
  return ret;
}

double tau(int i, int j, double gamma, int window_size) {
  if (i >= j) {
    return 0;
  }
  return (j - i) * gamma * std::log(window_size);
}

double log_factorial(int i) {
  return ::lgamma(i + 1);
}

double log_choose(int n, int k) {
  if (n < 0 || k < 0 || n < k) {
    return -INFINITY;
  }
  return log_factorial(n) - log_factorial(k) - log_factorial(n - k);
}

double sigma(double p, uint32_t d, uint32_t r) {
  double ret = log_choose(d, r);
  ret += r * std::log(p);
  ret += (d - r) * std::log(1 - p);
  return -ret;
}

double get_batch_weight(
    const std::vector<uint32_t>& d_vector,
    const std::vector<uint32_t>& r_vector,
    const std::vector<double>& p_vector,
    int batch_id) {
  double ret =
      sigma(p_vector[kBaseState], d_vector[batch_id], r_vector[batch_id]) -
        sigma(p_vector[kBurstState], d_vector[batch_id], r_vector[batch_id]);
  return (ret > 0) ? ret : 0;
}

std::pair<int, double> calc_previous_optimal_state(
    int now_state,
    double prev_base_optimal_cost,
    double prev_burst_optimal_cost,
    double gamma,
    int window_size) {
  // [previous] base state (optimal) -> [now] now_state
  const double prev_base_optimal_to_now_state_cost
    = prev_base_optimal_cost
    + tau(kBaseState, now_state, gamma, window_size);
  // [previous] burst state (optimal) -> [now] now_state
  const double prev_burst_optimal_to_now_state_cost
    = prev_burst_optimal_cost
    + tau(kBurstState, now_state, gamma, window_size);

  int prev_optimal_state = kBaseState;
  double prev_optimal_in_now_state_cost
    = prev_base_optimal_to_now_state_cost;
  if (prev_base_optimal_to_now_state_cost
      > prev_burst_optimal_to_now_state_cost) {
    prev_optimal_state = kBurstState;
    prev_optimal_in_now_state_cost
      = prev_burst_optimal_to_now_state_cost;
  }

  return std::make_pair(prev_optimal_state,
                        prev_optimal_in_now_state_cost);
}

bool check_branch_cuttable(
    const std::vector<uint32_t>& d_vector,
    const std::vector<uint32_t>& r_vector,
    const std::vector<double> & p_vector,
    int batch_id,
    double burst_cut_threshold) {
  const int window_size = d_vector.size();

#ifdef BURST_DEBUG
  std::cout
    << "    --- sigma(1) - sigma(0)="
    << sigma(p_vector[kBurstState],
             d_vector[batch_id],
             r_vector[batch_id])
    - sigma(p_vector[kBaseState],
            d_vector[batch_id],
            r_vector[batch_id])
    << ", "
    << "burst_cut_threshold * log(window_size)="
    <<  burst_cut_threshold * std::log(window_size)
    << std::endl;
#endif

  if (sigma(p_vector[kBurstState],
            d_vector[batch_id],
            r_vector[batch_id])
      - sigma(p_vector[kBaseState],
              d_vector[batch_id],
              r_vector[batch_id])
      > burst_cut_threshold * std::log(window_size)) {
    return true;
  }
  return false;
}

struct is_negative {
  bool operator()(double x) const {
    return x < 0;
  }
};

void erase_uncalc_batches(std::vector<double>& batch_weights) {
  std::vector<double>::iterator iter = std::remove_if(
      batch_weights.begin(), batch_weights.end(), is_negative());
  batch_weights.erase(iter, batch_weights.end());
}

}  // namespace

void burst_detect(const std::vector<uint32_t> & d_vector,
                  const std::vector<uint32_t> & r_vector,
                  std::vector<double>& batch_weights,
                  double scaling_param,
                  double gamma,
                  double burst_cut_threshold) {
  const int window_size = d_vector.size();
  try {
    if (gamma <= 0) {
      throw "gamma must be > 0.";
    }
    if (scaling_param <= 1) {
      throw "scaling_param must be > 1.";
    }
    if (burst_cut_threshold <= 0) {
      throw "burst_cut_threshold must be > 0.";
    }
    if (d_vector.size() != r_vector.size()) {
      throw "Error: d_vector.size() != r_vector.size()";
    }
    for (int batch_id = 0; batch_id < window_size; batch_id++) {
      if (d_vector[batch_id] < r_vector[batch_id]) {
        throw "Error: d_vector[batch_id] < r_vector[batch_id]";
      }
    }
  } catch (char const* e) {
    batch_weights.assign(window_size, kDefaultBatchWeight);
    std::cout << "Exception : " << e << std::endl;
    return;
  }
  const std::vector<double> p_vector
    = get_p_vector(d_vector, r_vector, scaling_param);

  erase_uncalc_batches(batch_weights);

  // exception handling of
  // - "p_{burst_state} > 1"
  // - "p_{base_state} = 0"
  if (1 < p_vector[kBurstState]) {
    batch_weights.resize(window_size, INFINITY);
    return;
  } else if (p_vector[kBaseState] == 0) {
    batch_weights.resize(window_size, 0);
    return;
  }

  const int reuse_batch_size = batch_weights.size();

  // the optimal costvals
  // - index 0: [1st batch - prev batch] optimal seq -> [now] base
  // - index 1: [1st batch - prev batch] optimal seq -> [now] burst
  double prev_optimal_in_now_states_costs[] = {-1, -1};

  // the optimal costval from 1st batch to previuous batch.
  // - index 0: previous : base
  // - index 1: previous : burst
  // To avoid "burst state in 1st batch",
  // we must set to INFINITY the "cost val of burst state".
  double prev_optimal_costs[] = {0, INFINITY};
  if (batch_weights.size() != 0 && 0 < batch_weights.back()) {
    // To avoid "base state in 1st batch",
    // we must set to INFINITY the "cost val of base state".
    prev_optimal_costs[kBaseState] = INFINITY;
    prev_optimal_costs[kBurstState] = 0;
  }

  // state sequences from 1st batch to now batch.
  // - index 0: [1st batch - prev batch] optimal seq -> [now] base
  // - index 1: [1st batch - prev batch] optimal seq -> [now] burst
  std::vector<std::vector<int> > prev_optimal_in_now_states_seq(kStatesNum);

  // state sequences from 1st batch to previous batch.
  // - index 0: [1st batch - prev batch] optimal seq & [prev] base
  // - index 1: [1st batch - prev batch] optimal seq & [prev] burst
  std::vector<std::vector<int> > prev_optimal_states_seq(kStatesNum);

  for (int update_batch_id = 0;
      update_batch_id < window_size - reuse_batch_size;
      update_batch_id++) {
#ifdef BURST_DEBUG
    std::cout << "- batch_id="
              << update_batch_id + reuse_batch_size
              << std::endl;
#endif

    for (int now_state = kBaseState; now_state < kStatesNum; now_state++) {
      std::pair<int, double> prev_optimal_pair;

#ifdef BURST_DEBUG
      std::cout << "  -- now state=" << now_state << std::endl;
#endif

      if ((0 < update_batch_id + reuse_batch_size) &&
         (d_vector[update_batch_id + reuse_batch_size - 1] == 0)) {
        // exception handling
        // in prev batch,
        // (d, r) = (0, 0)
#ifdef BURST_DEBUG
        std::cout << "    --- exception (d = 0)" << std::endl;
#endif

        prev_optimal_pair.first = kBaseState;
        prev_optimal_pair.second =
            prev_optimal_costs[kBaseState] +
            tau(kBaseState, now_state, gamma, window_size);
      } else if (0 < update_batch_id + reuse_batch_size &&
          check_branch_cuttable(d_vector, r_vector, p_vector,
                                update_batch_id + reuse_batch_size - 1,
                                burst_cut_threshold)) {
#ifdef BURST_DEBUG
        std::cout << "    --- cut->batch_id="
                  << update_batch_id + reuse_batch_size - 1
                  << std::endl;
#endif

        prev_optimal_pair.first = kBaseState;
        prev_optimal_pair.second =
            prev_optimal_costs[kBaseState] +
            tau(kBaseState, now_state, gamma, window_size);
      } else {
        prev_optimal_pair =
            calc_previous_optimal_state(now_state,
                                        prev_optimal_costs[kBaseState],
                                        prev_optimal_costs[kBurstState],
                                        gamma, window_size);
      }

      prev_optimal_in_now_states_costs[now_state] =
          prev_optimal_pair.second +
          sigma(p_vector[now_state],
                d_vector[update_batch_id + reuse_batch_size],
                r_vector[update_batch_id + reuse_batch_size]);

      prev_optimal_in_now_states_seq[now_state] =
          prev_optimal_states_seq[prev_optimal_pair.first];
      prev_optimal_in_now_states_seq[now_state].push_back(now_state);
    }

#ifdef BURST_DEBUG
    std::cout << "  -- base state cost="
              << prev_optimal_in_now_states_costs[kBaseState]
              << ", "
              << "burst state cost="
              << prev_optimal_in_now_states_costs[kBurstState]
              << std::endl;
#endif

    //
    // ready for precessing the next batch.
    //
    for (int state = kBaseState; state < kStatesNum; state++) {
      prev_optimal_costs[state] = prev_optimal_in_now_states_costs[state];
      prev_optimal_states_seq[state] =
          prev_optimal_in_now_states_seq[state];
    }
  }

#ifdef BURST_DEBUG
  std::cout << "- last burst_cut check"
            << std::endl;
#endif

  std::vector<int> optimal_states_seq;

  if (d_vector[window_size - 1] == 0) {
    // exception handling
    // in prev batch,
    // (d, r) = (0, 0)
#ifdef BURST_DEBUG
        std::cout << "    --- exception (d = 0)" << std::endl;
#endif
    optimal_states_seq = prev_optimal_in_now_states_seq[kBaseState];

  } else if (check_branch_cuttable(d_vector, r_vector, p_vector,
                                  window_size - 1,
                                  burst_cut_threshold)) {
#ifdef BURST_DEBUG
    std::cout << "    --- cut->batch_id="
              << window_size - 1
              << std::endl;
#endif
    optimal_states_seq = prev_optimal_in_now_states_seq[kBaseState];

  } else {
    optimal_states_seq =
        prev_optimal_in_now_states_costs[kBaseState] <=
            prev_optimal_in_now_states_costs[kBurstState] ?
          prev_optimal_in_now_states_seq[kBaseState] :
          prev_optimal_in_now_states_seq[kBurstState];
  }

  //
  // calculation of batch_weights
  //

  // reuse of past results
  for (int batch_id = 0; batch_id < reuse_batch_size; batch_id++) {
    if (0 < batch_weights[batch_id]) {
      batch_weights[batch_id] =
          get_batch_weight(d_vector, r_vector, p_vector, batch_id);
    }
  }
  // new calculation
  for (int batch_id = reuse_batch_size; batch_id < window_size; batch_id++) {
    int state = optimal_states_seq[batch_id - reuse_batch_size];
    batch_weights.push_back(state == kBurstState ?
                              get_batch_weight(d_vector, r_vector,
                                               p_vector, batch_id) :
                              0);
  }
}

}  // namespace engine
}  // namespace burst
}  // namespace core
}  // namespace jubatus
