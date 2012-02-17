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

#include <pficommon/data/unordered_map.h>
#include "../common/key_manager.hpp"
#include "recommender_type.hpp"

namespace jubatus {
namespace recommender {

class recommender;
class similarity_base;
class anchor_finder_base;
class anchor_builder_base;


class recommender_builder{
public:
  recommender_builder(const std::string& similarity_name,
		      const std::string& anchor_finder_name,
		      const std::string& anchor_builder_name) :
    similarity_name_(similarity_name),
    anchor_finder_name_(anchor_finder_name),
    anchor_builder_name_(anchor_builder_name) {}

  ~recommender_builder();

  //void add_row(const std::string& id, const sfv_t& sfv);
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);

  void get_diff(recommender_diff_t& ret) const;

  void build(const recommender& base,
             size_t all_anchor_num, 
             size_t anchor_num_per_data,
             recommender& r);

private:
  std::string similarity_name_;
  std::string anchor_finder_name_;
  std::string anchor_builder_name_;
  key_manager feature2id_;

  void canonalize_originals();
  pfi::data::unordered_map<std::string, sfvi_t> originals_;
};

} // namespace recommender
} // namespace jubatus
