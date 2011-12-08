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
#include "server_util.hpp"
#include "jubatus_serv.hpp"
#include "recommender_types.hpp"
#include "../storage/recommender_storage.hpp"

#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../recommender/recommender_base.hpp"

namespace jubatus {
namespace server {

typedef std::vector<std::pair<std::string, datum> > rows;

class recommender_serv : public jubatus_serv<storage::recommender_storage,int>
{
public:
  recommender_serv(const server_argv&);
  virtual ~recommender_serv();

  int set_config(config_data config);
  config_data get_config(int);

  int clear_row(std::string id, int);
  int update_row(std::string id, datum dat);
  int build(int);
  int clear(int);

  pfi::lang::shared_ptr<storage::recommender_storage> make_model();
  void after_load();

  datum complete_row_from_id(std::string id, int);
  datum complete_row_from_data(datum dat);
  similar_result similar_row_from_id(std::string id, size_t ret_num);
  similar_result similar_row_from_data(std::pair<datum, size_t>);

  datum decode_row(std::string id, int);
  rows get_all_rows(int);

private:
  void init();

  config_data config_;
  pfi::lang::shared_ptr<jubatus::recommender::recommender_base> recommender_;
  //  pfi::lang::shared_ptr<jubatus::recommender::recommender> recommender_;
  //  pfi::lang::shared_ptr<jubatus::recommender::recommender_builder> recommender_builder_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace server
} // namespace jubatus
