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

#include "storage_factory.hpp"

#include <string>
#include "../common/exception.hpp"
#include "compressive_storage.hpp"
#include "gmm_compressor.hpp"
#include "kmeans_compressor.hpp"
#include "simple_storage.hpp"

namespace jubatus {
namespace core {
namespace clustering {

jubatus::util::lang::shared_ptr<storage> storage_factory::create(
    const std::string& name,
    const clustering_config& config) {
  typedef jubatus::util::lang::shared_ptr<storage> ptr;
  ptr ret;
  if (config.compressor_method == "compressive_kmeans") {
    compressive_storage *s = new compressive_storage(name, config);
    s->set_compressor(jubatus::util::lang::shared_ptr<compressor::compressor>(
                          new compressor::kmeans_compressor(config)));
    ret.reset(s);
  } else if (config.compressor_method == "compressive_gmm") {
    compressive_storage *s = new compressive_storage(name, config);
    s->set_compressor(jubatus::util::lang::shared_ptr<compressor::compressor>(
                          new compressor::gmm_compressor(config)));
    ret.reset(s);
  } else if (config.compressor_method == "simple") {
    simple_storage *s = new simple_storage(name, config);
    ret.reset(s);
  } else {
    throw JUBATUS_EXCEPTION(
        common::unsupported_method(config.compressor_method));
  }
  return ret;
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
