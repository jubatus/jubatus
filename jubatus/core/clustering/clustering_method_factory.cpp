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

#include "clustering_method_factory.hpp"

#include <string>
#include "../common/exception.hpp"
#include "kmeans_clustering_method.hpp"
#include "gmm_clustering_method.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace clustering {

shared_ptr<clustering_method> clustering_method_factory::create(
    const std::string& method,
    const clustering_config& config) {
  if (method == "kmeans") {
    return shared_ptr<clustering_method>(
        new kmeans_clustering_method(config.k));
  } else if (method == "gmm") {
    return shared_ptr<clustering_method>(
        new gmm_clustering_method(config.k));
  }
  throw JUBATUS_EXCEPTION(core::common::unsupported_method(method));
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
