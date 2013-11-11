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

#ifndef JUBATUS_CORE_CLUSTERING_GMM_TYPES_HPP_
#define JUBATUS_CORE_CLUSTERING_GMM_TYPES_HPP_

#include <Eigen/Sparse>

#include <algorithm>
#include <vector>
#include <pficommon/lang/shared_ptr.h>

namespace jubatus {
namespace core {
namespace clustering {

typedef Eigen::SparseVector<double> eigen_svec_t;
typedef Eigen::SparseMatrix<double> eigen_smat_t;
typedef Eigen::SimplicialCholesky<eigen_smat_t> eigen_solver_t;
typedef std::vector<eigen_svec_t> eigen_svec_list_t;
typedef std::vector<eigen_smat_t> eigen_smat_list_t;
typedef std::vector<pfi::lang::shared_ptr<eigen_solver_t> > eigen_solver_list_t;

struct eigen_wsvec_t {
  double weight;
  eigen_svec_t data;
};
typedef std::vector<eigen_wsvec_t> eigen_wsvec_list_t;

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_GMM_TYPES_HPP_
