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

#pragma once

#include <stdint.h>
#include <map>
#include "recommender_base.hpp"
#include "../storage/lsh_index_storage.hpp"

namespace jubatus {
namespace recommender {

class euclid_lsh : public recommender_base {
 public:
  euclid_lsh();
  explicit euclid_lsh(const std::map<std::string, std::string>& config);
  euclid_lsh(uint64_t lsh_num,
             uint64_t table_num,
             float bin_width,
             uint32_t num_probe,
             uint32_t seed);
  ~euclid_lsh();

  virtual void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> >& ids, size_t ret_num) const;
  virtual void clear();
  virtual void clear_row(const std::string& id);
  virtual void update_row(const std::string& id, const sfv_diff_t& diff);
  virtual void get_all_row_ids(std::vector<std::string>& ids) const;

  virtual std::string type() const;
  virtual storage::recommender_storage_base* get_storage();
  virtual const storage::recommender_storage_base* get_const_storage() const;

 private:
  virtual bool save_impl(std::ostream& os);
  virtual bool load_impl(std::istream& is);

  void initialize_shift(size_t lsh_num, uint32_t seed);

  storage::lsh_index_storage lsh_index_;
  std::vector<float> shift_;
  const float bin_width_;
  const uint32_t num_probe_;
};

}
}
