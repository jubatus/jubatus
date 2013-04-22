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

#ifndef JUBATUS_CORE_RECOMMENDER_EUCLID_LSH_HPP_
#define JUBATUS_CORE_RECOMMENDER_EUCLID_LSH_HPP_

#include <stdint.h>
#include <utility>
#include <string>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/text/json.h>
#include "recommender_base.hpp"
#include "../storage/lsh_index_storage.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class euclid_lsh : public recommender_base {
 public:
  using recommender_base::similar_row;
  using recommender_base::neighbor_row;

  static const uint64_t DEFAULT_LSH_NUM;
  static const uint64_t DEFAULT_TABLE_NUM;
  static const float DEFAULT_BIN_WIDTH;
  static const uint32_t DEFAULT_NUM_PROBE;
  static const uint32_t DEFAULT_SEED;
  static const bool DEFAULT_RETAIN_PROJECTION;

  struct config {
    config();

    int64_t lsh_num;
    int64_t table_num;
    float bin_width;
    int32_t probe_num;
    int32_t seed;
    bool retain_projection;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(lsh_num) & MEMBER(table_num) & MEMBER(bin_width) &
        MEMBER(probe_num) & MEMBER(seed) & MEMBER(retain_projection);
    }
  };

  euclid_lsh();
  explicit euclid_lsh(const config& config);
  ~euclid_lsh();

  virtual void neighbor_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void neighbor_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;

  virtual void similar_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void similar_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;

  virtual void clear();
  virtual void clear_row(const std::string& id);
  virtual void update_row(const std::string& id, const sfv_diff_t& diff);
  virtual void get_all_row_ids(std::vector<std::string>& ids) const;

  virtual std::string type() const;
  virtual core::storage::lsh_index_storage* get_storage();
  virtual const core::storage::lsh_index_storage* get_const_storage() const;

 private:
  friend class pfi::data::serialization::access;
  template <typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(lsh_index_) & MEMBER(bin_width_) & MEMBER(num_probe_) &
      MEMBER(projection_) & MEMBER(retain_projection_);
  }

  std::vector<float> calculate_lsh(const sfv_t& query);
  std::vector<float> get_projection(uint32_t seed);

  virtual bool save_impl(std::ostream& os);
  virtual bool load_impl(std::istream& is);

  core::storage::lsh_index_storage lsh_index_;
  float bin_width_;
  uint32_t num_probe_;

  pfi::data::unordered_map<uint32_t, std::vector<float> > projection_;
  bool retain_projection_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_EUCLID_LSH_HPP_
