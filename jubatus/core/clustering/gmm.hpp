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

#ifndef JUBATUS_CORE_CLUSTERING_GMM_HPP_
#define JUBATUS_CORE_CLUSTERING_GMM_HPP_

#include "gmm_types.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class gmm {
 public:
  void batch(const eigen_wsvec_list_t& data, int d, int k);
  eigen_svec_list_t get_centers() {
      return means_;
  }
  eigen_smat_list_t get_covs() {
      return covs_;
  }
  eigen_svec_t get_nearest_center(const eigen_svec_t& p) const;
  int64_t get_nearest_center_index(const eigen_svec_t& p) const;

 private:
  void initialize(const eigen_wsvec_list_t& data, int d, int k);
  bool is_converged(
      int64_t niter,
      const eigen_svec_list_t& means,
      const eigen_svec_list_t& old_means,
      double obj,
      double old_obj);
  eigen_svec_t cluster_probs(
      const eigen_svec_t& x,
      const eigen_svec_list_t& mean,
      const eigen_smat_list_t& cov,
      const eigen_solver_list_t& solvers) const;
  eigen_svec_list_t   means_;
  eigen_smat_list_t   covs_;
  eigen_smat_t eye_;
  eigen_solver_list_t cov_solvers_;
  int d_;
  int k_;
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_GMM_HPP_
