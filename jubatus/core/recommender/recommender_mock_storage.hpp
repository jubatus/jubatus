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

#ifndef JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_STORAGE_HPP_
#define JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_STORAGE_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <msgpack.hpp>
#include "jubatus/util/data/serialization.h"
#include "../framework/mixable.hpp"
#include "../common/version.hpp"
#include "recommender_type.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class recommender_mock_storage {
 public:
  virtual ~recommender_mock_storage();

  void set_similar_items(
      const common::sfv_t& query,
      const std::vector<std::pair<std::string, float> >& ids);
  void set_neighbor_items(
      const common::sfv_t& query,
      const std::vector<std::pair<std::string, float> >& ids);

  void similar_items_similarity(
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void neighbor_items_distance(
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;

  void update(const common::sfv_t& from, const common::sfv_t& to);
  void remove(const common::sfv_t& query);
  void clear();

  std::string name() const;

  storage::version get_version() const {
    return storage::version();
  }

  void pack(msgpack::packer<msgpack::sbuffer>& packer) const;
  void unpack(msgpack::object o);

  void get_diff(recommender_mock_storage& diff) const;
  bool put_diff(const recommender_mock_storage& mixed_diff);
  void mix(const recommender_mock_storage& lhs, recommender_mock_storage& rhs)
      const;

 private:
  typedef std::map<common::sfv_t, std::vector<std::pair<std::string, float> > >
    relation_type;

  friend class jubatus::util::data::serialization::access;
  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(similar_relation_) & JUBA_MEMBER(neighbor_relation_);
  }

  static void get_relation(
      const common::sfv_t& query,
      const relation_type& relmap,
      size_t ret_num,
      std::vector<std::pair<std::string, float> >& ids);

  static void update_relation_key(
      const common::sfv_t& from,
      const common::sfv_t& to,
      relation_type& relmap);

  static void mix_relation(const relation_type& from, relation_type& to);

  relation_type similar_relation_;
  relation_type neighbor_relation_;

 public:
  MSGPACK_DEFINE(similar_relation_, neighbor_relation_);
};

typedef framework::delegating_mixable<
    recommender_mock_storage, recommender_mock_storage>
    mixable_recommender_mock_storage;

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_RECOMMENDER_MOCK_STORAGE_HPP_
