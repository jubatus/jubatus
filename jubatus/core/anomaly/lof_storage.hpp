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

#include <msgpack.hpp>
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/data/unordered_set.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/text/json.h"

#include "../common/type.hpp"
#include "../common/unordered_map.hpp"
#include "../framework/mixable.hpp"
#include "../recommender/recommender_base.hpp"
#include "../recommender/recommender_factory.hpp"

namespace jubatus {
namespace core {
namespace anomaly {

struct lof_entry {
  float kdist;
  float lrd;

  MSGPACK_DEFINE(kdist, lrd);

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(kdist) & JUBA_MEMBER(lrd);
  }
};

typedef jubatus::util::data::unordered_map<std::string, lof_entry> lof_table_t;

class lof_storage {
 public:
  static const uint32_t DEFAULT_NEIGHBOR_NUM;
  static const uint32_t DEFAULT_REVERSE_NN_NUM;

  struct config {
    config();

    int nearest_neighbor_num;
    int reverse_nearest_neighbor_num;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar
          & JUBA_MEMBER(nearest_neighbor_num)
          & JUBA_MEMBER(reverse_nearest_neighbor_num);
    }
  };

  lof_storage();
  explicit lof_storage(
      jubatus::util::lang::shared_ptr<core::recommender::recommender_base>
          nn_engine);

  // config contains parameters for the underlying nearest neighbor search
  explicit lof_storage(
      const config& config,
      jubatus::util::lang::shared_ptr<core::recommender::recommender_base>
          nn_engine);

  virtual ~lof_storage();

  // For Analyze
  // calculate lrd of query and lrd values of its neighbors
  float collect_lrds(
      const common::sfv_t& query,
      jubatus::util::data::unordered_map<std::string, float>&
          neighbor_lrd) const;
  float collect_lrds(
      const std::string& id,
      jubatus::util::data::unordered_map<std::string, float>&
          neighbor_lrd) const;

  // For Update
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;
  void update_row(const std::string& row, const common::sfv_t& diff);

  void update_all();  // Update kdists and lrds

  std::string name() const;

  // getter & setter & update for kdist and lrd values
  float get_kdist(const std::string& row) const;
  float get_lrd(const std::string& row) const;

  // just for test
  void set_nn_engine(
      jubatus::util::lang::shared_ptr<core::recommender::recommender_base>
          nn_engine);

  void get_diff(lof_table_t& diff) const;
  bool put_diff(const lof_table_t& mixed_diff);
  void mix(const lof_table_t& lhs, lof_table_t& rhs) const;

  storage::version get_version() const {
    return storage::version();
  }

  template<class Buffer>
  void pack(msgpack::packer<Buffer>& packer) const {
    packer.pack(*this);
  }
  void unpack(msgpack::object o) {
    o.convert(this);
  }

  MSGPACK_DEFINE(lof_table_, lof_table_diff_, neighbor_num_, reverse_nn_num_);

 private:
  static void mark_removed(lof_entry& entry);
  static bool is_removed(const lof_entry& entry);

  friend class jubatus::util::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(lof_table_) & JUBA_MEMBER(lof_table_diff_);
    ar & JUBA_MEMBER(neighbor_num_) & JUBA_MEMBER(reverse_nn_num_);
  }

  float collect_lrds_from_neighbors(
      const std::vector<std::pair<std::string, float> >& neighbors,
      jubatus::util::data::unordered_map<std::string, float>&
          neighbor_lrd) const;

  void collect_neighbors(
      const std::string& row,
      jubatus::util::data::unordered_set<std::string>& nn) const;

  void update_entries(
      const jubatus::util::data::unordered_set<std::string>& rows);
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

  jubatus::util::lang::shared_ptr<core::recommender::recommender_base>
    nn_engine_;
};

typedef framework::delegating_mixable<lof_storage, lof_table_t>
    mixable_lof_storage;

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_LOF_STORAGE_HPP_
