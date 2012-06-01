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

#include <iosfwd>
#include <string>
#include <vector>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include "lsh_vector.hpp"
#include "recommender_storage_base.hpp"
#include "storage_type.hpp"
#include "../common/key_manager.hpp"

namespace jubatus {
namespace storage{

class lsh_index_storage : public recommender_storage_base {
public:
  lsh_index_storage();
  lsh_index_storage(size_t lsh_num, size_t table_num, uint32_t seed);
  lsh_index_storage(size_t table_num, const std::vector<float>& shift);
  virtual ~lsh_index_storage();

  // hash is a randomly-projected and scaled hash values without shifting
  void set_row(const std::string& row, const std::vector<float>& hash, float norm);
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;

  void similar_row(const std::vector<float>& hash,
                   float norm,
                   uint64_t probe_num,
                   uint64_t ret_num,
                   std::vector<std::pair<std::string, float> >& ids) const;
  std::string name() const;

  size_t table_num() const { return table_num_; }
  size_t all_lsh_num() const { return shift_.size(); }

  bool save(std::ostream& os);
  bool load(std::istream& is);

  virtual void get_diff(std::string& diff) const;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff);
  virtual void mix(const std::string& lhs, std::string& rhs) const;

private:
  typedef pfi::data::unordered_multimap<uint64_t, uint64_t> lsh_table_t;

  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(master_table_) & MEMBER(master_table_diff_)
        & MEMBER(shift_) & MEMBER(table_num_);

    if (ar.is_read) {
      for (lsh_master_table_t::iterator it = master_table_diff_.begin(); it != master_table_.end(); ++it) {
        add_index(it->first, it->second);
      }
      for (lsh_master_table_t::iterator it = master_table_.begin(); it != master_table_.end(); ++it) {
        if (master_table_diff_.find(it->first) == master_table_diff_.end()) {
          add_index(it->first, it->second);
        }
      }
    }
  }

  std::vector<float> make_entry(const std::vector<float>&hash,
                                float norm,
                                lsh_entry& entry) const;
  void add_index(const std::string& row, const lsh_entry& entry);
  bool retrieve_hit_rows(const lsh_vector& key,
                         size_t ret_num,
                         pfi::data::unordered_set<uint64_t>& cands) const;
  void get_sorted_similar_rows(const pfi::data::unordered_set<uint64_t>& cands,
                               const bit_vector& query_simhash,
                               float query_norm,
                               uint64_t ret_num,
                               std::vector<std::pair<std::string, float> >& ids) const;
  const lsh_entry* get_lsh_entry(const std::string& row) const;
  void remove_model_row(const std::string& row);
  void set_mixed_row(const std::string& row, const lsh_entry& entry);

  lsh_master_table_t master_table_;
  lsh_master_table_t master_table_diff_;

  lsh_table_t lsh_table_;

  std::vector<float> shift_;
  uint64_t table_num_;
  key_manager key_manager_;
};

}
}
