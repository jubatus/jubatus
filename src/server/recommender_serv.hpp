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
#include <vector>

#include <pficommon/lang/shared_ptr.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>

#include "./recommender/recommender.hpp"
#include "./recommender/recommender_builder.hpp"
#include "./fv_converter/datum_to_fv_converter.hpp"

#include "recommender_rpc.hpp"
#include "server_util.hpp"

namespace jubatus {
namespace recommender {

class server : public jubatus::jubatus_serv
{
public:
  server(const server_argv&);
  ~server();

  result<int> set_config(std::string,config_data config);
  result<config_data> get_config(std::string);

  result<int> clear_row(std::string,std::vector<std::string> ids);
  result<int> update_row(std::string,rows dat);
  result<int> build(std::string);
  result<int> clear(std::string);
  result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > get_status(std::string);

  result<datum> complete_row_from_id(std::string,std::string id);
  result<datum> complete_row_from_data(std::string,datum dat);
  result<similar_result> similar_row_from_id(std::string,std::string id, size_t ret_num);
  result<similar_result> similar_row_from_data(std::string,datum dat, size_t ret_num);

  result<datum> decode_row(std::string,std::string id);
  result<rows> get_all_rows(std::string);

  recommender_diff_t get_diff(int);
  int put_diff(recommender_data); // diff demo nandemo ne-kedona!

  void mix(const std::vector<std::pair<std::string, int> >&);
  void bind_all_methods(mprpc_server&, const std::string&, int);

private:
  void init();

  config_data config_;
  pfi::lang::shared_ptr<recommender> recommender_;
  pfi::lang::shared_ptr<recommender_builder> recommender_builder_;
  pfi::lang::shared_ptr<datum_to_fv_converter> converter_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace recommender
} // namespace jubatus
