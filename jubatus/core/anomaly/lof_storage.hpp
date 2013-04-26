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

#ifndef JUBATUS_CORE_ANOMALY_LOF_STORAGE_HPP_
#define JUBATUS_CORE_ANOMALY_LOF_STORAGE_HPP_

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/text/json.h>

#include "anomaly_storage_base.hpp"
#include "../common/type.hpp"
#include "../recommender/recommender_base.hpp"
#include "../recommender/recommender_factory.hpp"

namespace jubatus {
namespace core {
namespace storage {

class lof_storage : public anomaly_storage_base {
 public:
  static const uint32_t DEFAULT_NEIGHBOR_NUM;
  static const uint32_t DEFAULT_REVERSE_NN_NUM;

  struct config {
    config();

    int nearest_neighbor_num;
    int reverse_nearest_neighbor_num;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(nearest_neighbor_num) & MEMBER(reverse_nearest_neighbor_num);
    }
  };

  lof_storage();
  explicit lof_storage(core::recommender::recommender_base* nn_engine);

  // config contains parameters for the underlying nearest neighbor search
  explicit lof_storage(
      const config& config,
      core::recommender::recommender_base* nn_engine);

  virtual ~lof_storage();

  // For Analyze
  // calculate lrd of query and lrd values of its neighbors
  float collect_lrds(
      const sfv_t& query,
      pfi::data::unordered_map<std::string, float>& neighbor_lrd) const;
  float collect_lrds(
      const std::string& id,
      pfi::data::unordered_map<std::string, float>& neighbor_lrd) const;

  // For Update
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;
  void update_row(const std::string& row, const sfv_t& diff);

  void update_all();  // Update kdists and lrds

  std::string name() const;

  // getter & setter & update for kdist and lrd values
  float get_kdist(const std::string& row) const;
  float get_lrd(const std::string& row) const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

  // just for test
  void set_nn_engine(core::recommender::recommender_base* nn_engine);

  virtual void get_diff(std::string& diff) const;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff);
  virtual void mix(const std::string& lhs, std::string& rhs) const;

 private:
  struct lof_entry {
    float kdist;
    float lrd;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(kdist) & MEMBER(lrd);
    }
  };

  typedef pfi::data::unordered_map<std::string, lof_entry> lof_table_t;

  static void mark_removed(lof_entry& entry);
  static bool is_removed(const lof_entry& entry);

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(lof_table_) & MEMBER(lof_table_diff_);
    ar & MEMBER(neighbor_num_) & MEMBER(reverse_nn_num_);

    // TODO(kashihara): Make it more efficient
    if (ar.is_read) {
      std::string name;
      ar & name;
      nn_engine_->clear();

      std::string impl;
      ar & impl;
      std::istringstream iss(impl);
      nn_engine_->load(iss);
    } else {
      std::string name = nn_engine_->type();
      ar & name;

      std::ostringstream oss;
      nn_engine_->save(oss);
      std::string str = oss.str();
      ar & str;
    }
  }

  float collect_lrds_from_neighbors(
      const std::vector<std::pair<std::string, float> >& neighbors,
      pfi::data::unordered_map<std::string, float>& neighbor_lrd) const;

  void serialize_diff(
      const lof_table_t& table,
      const std::string& nn_diff,
      std::ostream& out) const;
  void deserialize_diff(
      const std::string& diff,
      lof_table_t& table,
      std::string& nn_diff) const;

  void collect_neighbors(
      const std::string& row,
      pfi::data::unordered_set<std::string>& nn) const;

  void update_entries(const pfi::data::unordered_set<std::string>& rows);
  void update_kdist(const std::string& row);
  void update_lrd(const std::string& row);

  void update_kdist_with_neighbors(
      const std::string& row,
      const std::vector<std::pair<std::string, float> >& neighbors);
  void update_lrd_with_neighbors(
      const std::string& row,
      const std::vector<std::pair<std::string, float> >& neighbors);

  lof_table_t lof_table_;  // table for storing k-dist and lrd values
  lof_table_t lof_table_diff_;

  uint32_t neighbor_num_;  // k of k-nn
  uint32_t reverse_nn_num_;  // ck of ck-nn as an approx. of k-reverse-nn

  pfi::lang::scoped_ptr<core::recommender::recommender_base> nn_engine_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_LOF_STORAGE_HPP_
