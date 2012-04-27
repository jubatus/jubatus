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
#include "lsh_vector.hpp"
#include "recommender_storage_base.hpp"
#include "storage_type.hpp"

namespace jubatus {
namespace storage{

class lsh_index_storage : public recommender_storage_base {
public:
  lsh_index_storage();
  explicit lsh_index_storage(size_t num_tables);
  virtual ~lsh_index_storage();

  float get_shift(size_t index) const;

  void set_row(const std::string& row, const lsh_vector& lv);
  void get_row(const std::string& row, lsh_vector& lv) const;
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;

  void similar_row(const lsh_vector& lv, std::vector<std::pair<std::string, float> >& ids, uint64_t ret_num) const;
  std::string name() const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

  virtual void get_diff(std::string& diff) const;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff);
  virtual void mix(const std::string& lhs, std::string& rhs) const;

private:
  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(master_table_) & MEMBER(master_table_diff_);

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

    // ar & MEMBER(master_table_) & MEMBER(master_table_diff_) & MEMBER(lsh_tables_);
  }

  lsh_master_table_t::iterator remove_row_and_get_iterator(const std::string& row);
  void add_index(const std::string& row, const lsh_vector& lv);
  float get_score(const std::string& row, const lsh_vector& lv) const;
  void remove_model_row(const std::string& row);
  void set_mixed_row(const std::string& row, const lsh_vector& lv);

  lsh_master_table_t master_table_;
  lsh_master_table_t master_table_diff_;
  std::vector<lsh_table_t> lsh_tables_;
};

}
}
