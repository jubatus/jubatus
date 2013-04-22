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

#ifndef JUBATUS_CORE_STORAGE_LSH_UTIL_HPP_
#define JUBATUS_CORE_STORAGE_LSH_UTIL_HPP_

#include <functional>
#include <queue>
#include <utility>
#include <vector>
#include "lsh_vector.hpp"

namespace jubatus {
namespace core {
namespace storage {

class lsh_probe_generator {
 public:
  lsh_probe_generator(const std::vector<float>& hash, size_t num_hash_tables);

  void init();

  lsh_vector base_all() const;
  const lsh_vector& base(size_t i) const {
    return base_[i];
  }
  std::pair<size_t, lsh_vector> get_next_table_and_vector();

 private:
  typedef std::pair<float, std::pair<size_t, std::vector<int> > > diff_type;
  typedef std::priority_queue<
      diff_type,
      std::vector<diff_type>,
      std::greater<diff_type> > heap_type;

  void next_perturbations();

  std::vector<std::vector<float> > hash_;
  std::vector<lsh_vector> base_;
  std::vector<std::vector<std::pair<float, int> > > perturbation_sets_;
  heap_type heap_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_LSH_UTIL_HPP_
