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

#ifndef JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_SERIALIZER_HPP_
#define JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_SERIALIZER_HPP_

#include <string>
#include "clustering_method.hpp"
#include "kmeans_clustering_method.hpp"
#include "clustering_config.hpp"
#include "../common/exception.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class clustering_method_serializer {
 public:
  template <class Archive, class ConcreteMethodType>
  static void do_serialize(Archive &ar, clustering_method &method) {
    ConcreteMethodType *t = dynamic_cast<ConcreteMethodType *>(&method);
    if (t == NULL) {
      throw JUBATUS_EXCEPTION(
          common::exception::runtime_error("Bad method type"));
    }
    t->serialize(ar);
  }
  template <class Archive>
  static void serialize(
      const std::string& method_name, Archive &ar, clustering_method &method) {
    if (method_name == "kmeans") {
      do_serialize<Archive, kmeans_clustering_method>(ar, method);
    } else {
      throw JUBATUS_EXCEPTION(common::unsupported_method(method_name));
    }
  }
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_SERIALIZER_HPP_
