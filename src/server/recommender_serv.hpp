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
#include "../framework/mixable.hpp"

using jubatus::recommender::recommender_base;

namespace jubatus {
namespace server {

struct rcmdr : public jubatus::framework::mixable<recommender_base, std::string>{
  rcmdr(){
    function<std::string(const recommender_base*)> getdiff(&get_diff);
    function<int(const recommender_base*, const std::string&, std::string&)> reduce(&rcmdr::reduce);
    function<int(recommender_base*, const std::string&)> putdiff(&put_diff);
    set_mixer(getdiff, reduce, putdiff);
  }
  static std::string get_diff(const recommender_base* model){
    std::string ret;
    model->get_const_storage()->get_diff(ret);
    return ret;
  }
  static int put_diff(recommender_base* model, std::string v){
    model->get_storage()->set_mixed_and_clear_diff(v);
    return 0;
  }
  static int reduce(const recommender_base* model, const std::string& v, std::string& acc){
    model->get_const_storage()->mix(v, acc);
    return 0;
  }
  virtual ~rcmdr(){}
  void clear(){}
};

typedef std::vector<std::pair<std::string, jubatus::datum> > rows;

class recommender_serv : public framework::jubatus_serv
{
public:
  recommender_serv(const framework::server_argv&);
  virtual ~recommender_serv();

  int set_config(config_data config);
  config_data get_config(int);

  int clear_row(std::string id, int);
  int update_row(std::string id, datum dat);
  int clear(int);

  pfi::lang::shared_ptr<recommender_base> make_model();
  void after_load();

  datum complete_row_from_id(std::string id, int);
  datum complete_row_from_data(datum dat);
  similar_result similar_row_from_id(std::string id, size_t ret_num);
  similar_result similar_row_from_data(std::pair<datum, size_t>);

  datum decode_row(std::string id, int);
  std::vector<std::string> get_all_rows(int);

  std::map<std::string, std::map<std::string, std::string> > get_status(int);

private:

  config_data config_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  rcmdr rcmdr_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace server
} // namespace jubatus
