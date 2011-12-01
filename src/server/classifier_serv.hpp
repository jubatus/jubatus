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

#include "../common/rpc_util.hpp"
#include "../classifier/classifier_base.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../storage/storage_base.hpp"

#include "classifier_types.hpp"
#include "jubatus_serv.hpp"
#include "diffv.hpp"


namespace jubatus{
namespace server{

class classifier_serv : public jubatus_serv<storage::storage_base,diffv>
{
public:
  classifier_serv(int args, char** argv);  
  virtual ~classifier_serv();

  storage::storage_base* make_model(const server_argv&);

  // msgpack only
  std::string get_storage(int);
  static diffv get_diff(const storage::storage_base*);
  static int put_diff(storage::storage_base*, diffv v);
  static int reduce(const storage::storage_base*, const diffv&, diffv&);

  int set_config(config_data);
  config_data get_config(int );
  int train(std::vector<std::pair<std::string, datum> > data);
  std::vector<std::vector<estimate_result> > classify(std::vector<datum> data);

  pfi::lang::shared_ptr<storage::storage_base> before_load();
  void after_load();

  std::map<std::pair<std::string, int>, std::map<std::string, std::string> > get_status(int);

private:
  config_data config_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<classifier_base> classifier_;

};

void mix_parameter(diffv& lhs, const diffv& rhs);
}
} // namespace jubatus
