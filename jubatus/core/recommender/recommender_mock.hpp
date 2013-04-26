// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_HPP_
#define JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include "recommender_base.hpp"
#include "recommender_mock_storage.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class recommender_mock : public recommender_base {
 public:
  using recommender_base::similar_row;
  using recommender_base::neighbor_row;

  recommender_mock();
  virtual ~recommender_mock();

  void set_similar_relation(
      const sfv_t& query,
      const std::vector<std::pair<std::string, float> >& ids);
  void set_similar_relation(
      const std::string& id,
      const std::vector<std::pair<std::string, float> >& ids);
  void set_neighbor_relation(
      const sfv_t& query,
      const std::vector<std::pair<std::string, float> >& ids);
  void set_neighbor_relation(
      const std::string& id,
      const std::vector<std::pair<std::string, float> >& ids);

  virtual void similar_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void neighbor_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void clear();
  virtual void clear_row(const std::string& id);
  virtual void update_row(const std::string& id, const sfv_diff_t& diff);
  virtual void get_all_row_ids(std::vector<std::string>& ids) const;

  virtual std::string type() const;
  virtual core::storage::recommender_storage_base* get_storage();
  virtual const core::storage::recommender_storage_base*
      get_const_storage() const;

 protected:
  virtual bool save_impl(std::ostream&);
  virtual bool load_impl(std::istream&);

 private:
  friend class pfi::data::serialization::access;
  template<typename Ar>
  void serialize(Ar& ar) {
    ar & orig_ & storage_;
  }

  recommender_mock_storage storage_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_HPP_
