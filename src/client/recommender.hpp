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

#include <string>
#include <map>
#include <vector>

#include <pficommon/math/random.h>
#include <pficommon/lang/scoped_ptr.h>

#include "../common/exception.hpp"
#include "../server/recommender_rpc.hpp"

namespace jubatus {
namespace client {

class recommender_impl;

class recommender {
public:
  recommender(const std::string &hosts, const std::string &name,
              double timeout, bool use_jubakeepers = true);
  ~recommender();

  void set_config(const jubatus::recommender::config_data& config);
  jubatus::recommender::config_data get_config();

  void clear_row(const std::vector<std::string>& ids);
  void update_row(const jubatus::recommender::rows& dat);
  void build(); // build() is only for standalone mode

  datum complete_row_from_id(const std::string& id);
  datum complete_row_from_data(const datum& dat);
  jubatus::recommender::similar_result similar_row_from_id(const std::string& id, size_t ret_num);
  jubatus::recommender::similar_result similar_row_from_data(const datum& dat, size_t ret_num);

  datum decode_row(const std::string& id);
  jubatus::recommender::rows get_all_rows();

  // call 'save' on all servers (the timing is not serialized)
  void save(const std::string& type, const std::string& id);
  void load(const std::string& type, const std::string& id);

private:
  pfi::lang::scoped_ptr<recommender_impl> impl_;
};

} //namespace client
} //namespace jubatus

