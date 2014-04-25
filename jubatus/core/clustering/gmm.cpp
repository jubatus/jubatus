// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "gmm.hpp"

#include <algorithm>
#include <ctime>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include "jubatus/util/math/random.h"

using jubatus::util::lang::shared_ptr;
using std::max;
using std::min;
using std::exp;
using std::vector;

namespace jubatus {
namespace core {
namespace clustering {

void gmm::batch(const eigen_wsvec_list_t& data, int d, int k) {
  if (data.empty()) {
    *this = gmm();
    return;
  }

  typedef eigen_wsvec_list_t::const_iterator data_iter;
  initialize(data, d, k);

  eigen_svec_list_t old_means;
  eigen_smat_list_t old_covs;
  eigen_solver_list_t old_solvers;
  double old_obj = 0, obj = 0;
  vector<double> weights(k);

  bool converged = false;
  int64_t niter = 1;
  while (!converged) {
    old_covs = covs_;
    old_means = means_;
    old_solvers = cov_solvers_;
    old_obj = obj;
    obj = 0;
    fill(weights.begin(), weights.end(), 0);
    fill(means_.begin(), means_.end(), eigen_svec_t(d));
    fill(covs_.begin(), covs_.end(), eigen_smat_t(d, d));

    for (data_iter i = data.begin(); i != data.end(); ++i) {
      eigen_svec_t cps =
          cluster_probs(i->data, old_means, old_covs, old_solvers);
      for (int c = 0; c < k; ++c) {
        double cp = i->weight * cps.coeff(c);
        means_[c] += cp * i->data;
        covs_[c] += i->data * (i->data.transpose()) * cp;
        weights[c] += cp;
        obj -= std::log(cp);
      }
    }
    for (int c = 0; c < k; ++c) {
      means_[c] /= weights[c];
      covs_[c] /= weights[c];
      double eps = 0.1;
      covs_[c] -= means_[c] * means_[c].transpose();
      covs_[c] += eps * eye_;
      cov_solvers_[c] =
          shared_ptr<eigen_solver_t>(new eigen_solver_t(covs_[c]));
    }
    converged = is_converged(niter++, means_, old_means, obj, old_obj);
  }
}

eigen_svec_t gmm::get_nearest_center(const eigen_svec_t& p) const {
  return means_[get_nearest_center_index(p)];
}

int64_t gmm::get_nearest_center_index(const eigen_svec_t& p) const {
  double max_prob = 0;
  int64_t max_idx = 0;
  eigen_svec_t cps = cluster_probs(p, means_, covs_, cov_solvers_);
  for (int c = 0; c < k_; ++c) {
    double cp = cps.coeff(c);
    if (cp > max_prob) {
      max_prob = cp;
      max_idx = c;
    }
  }
  return max_idx;
}

void gmm::initialize(const eigen_wsvec_list_t& data, int d, int k) {
  d_ = d;
  k_ = k;
  means_ = eigen_svec_list_t(k);
  covs_ = eigen_smat_list_t(k, eigen_smat_t(d, d));
  cov_solvers_ = eigen_solver_list_t(k);
  eye_ = eigen_smat_t(d, d);

  for (int i = 0; i < d; ++i) {
    eye_.insert(i, i) = 1;
  }

  jubatus::util::math::random::mtrand r(time(NULL));
  for (int c = 0; c < k; ++c) {
    means_[c] = data[r.next_int(0, data.size()-1)].data;
    for (int i = 0; i < d; ++i) {
      covs_[c].insert(i, i) = 1;
    }
    cov_solvers_[c] = shared_ptr<eigen_solver_t>(new eigen_solver_t(covs_[c]));
  }
}

bool gmm::is_converged(
    int64_t niter,
    const eigen_svec_list_t& means,
    const eigen_svec_list_t& old_means,
    double obj,
    double old_obj) {
  double max_dist = 0;
  for (int c = 0; c < k_; ++c) {
    max_dist = max(max_dist, (means[c] - old_means[c]).norm());
  }
  return (max_dist < 1e-09 || niter > 1e05);
}

eigen_svec_t gmm::cluster_probs(
    const eigen_svec_t& x,
    const eigen_svec_list_t& means,
    const eigen_smat_list_t& covs,
    const eigen_solver_list_t& cov_solvers) const {
  double den = DBL_MIN;
  eigen_svec_t ret(k_);
  for (int i = 0; i < k_; ++i) {
    eigen_svec_t dif = x - means[i];
    double det = std::abs(cov_solvers[i]->determinant());
    double quad = (dif.transpose() * cov_solvers[i]->solve(dif)).sum();
    double lp = -1 / 2. * (std::log(det) + quad);
    ret.coeffRef(i) = lp;
    den = (den == DBL_MIN) ?
        lp : std::max(den, lp) +
            std::log(1 + std::exp(min(den, lp) - std::max(den, lp)));
  }
  for (int i = 0; i < k_; ++i) {
    ret.coeffRef(i) = std::exp(ret.coeff(i) - den);
  }
  return ret;
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
