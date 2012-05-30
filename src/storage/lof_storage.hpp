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
#include "../common/type.hpp"
#include "anomaly_storage_base.hpp"
#include "storage_type.hpp"


namespace jubatus {
namespace storage{

class lof_storage : public anomaly_storage_base {
public:
  lof_storage();
  lof_storage(const std::map<std::string, std::string>& config); // config contains parameters for the underlying nearest neighbor search
  virtual ~lof_storage();

  // For Analyze
  virtual void calc_lrd(const sfv_t& query, std::pair<std::string, float>& lrd_value, size_t neighbor_num); // calculate lrd value for an unseen instance coming in Analyze

  virtual void similar_row_kdists(const std::string& row, std::map<std::string, float>& neighbor_lrd_values, size_t neighbor_num); // call similar_row and get_kdist iteratively for collectin kdist values of neighbors
  virtual void similar_row_lrds(const std::string& row, std::map<std::string, float>& neighbor_lrd_values, size_t neighbor_num); // call similar_row and get_lrd iteratively for collecting lrd values of neighbors
  
  // For Update
  virtual void set_row(std::string& row, const sfv_t& query); // can be merged with update_row?
  // virtual void decode_row(const std::string& row, sfv_t& corresponding_vector); // no need?
  virtual void remove_row(const std::string& row);
  virtual void clear();
  virtual void get_all_row_ids(std::vector<std::string>& ids) const;

  virtual void update_row(std::string& row, const sfv_t& diff);
  virtual void similar_row(const std::string& row, std::vector<std::pair<std::string, float> >& ids, size_t neighbor_num) const; 

  virtual std::string name() const;

  // getter & setter & update for kdist and lrd values
  virtual void get_kdist(std::pair<std::string, float>& kdist_value) const;
  virtual void get_lrd(std::pair<std::string, float>& lrd_value) const;

  virtual void set_kdist(const std::pair<std::string, float>& kdist_value);
  virtual void set_lrd(const std::pair<std::string, float>& lrd_value);

  virtual void update_all_kdist();
  virtual void update_all_lrd();

  virtual void update_lrd(const std::string& row);
  virtual void update_kdist(const std::string& row);

  bool save(std::ostream& os);
  bool load(std::istream& is);

  virtual void get_diff(std::string& diff) const;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff);
  virtual void mix(const std::string& lhs, std::string& rhs) const;

private:
  uint32_t neighbor_num_; // k of k-nn (no need?)
  float reverse_nn_coefficient_; // c of ck-nn as an approx. of k-reverse-nn (no need?)
  // storage::lof_table_t lof_table_ ; // table for storing k-dist and lrd values

  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    
  }

  // add anything you want
};

}
}
