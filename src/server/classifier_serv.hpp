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

#include "classifier_rpc.hpp"
#include "server_util.hpp"

namespace jubatus{
namespace classifier {

class server : public jubatus::jubatus_serv
{
public:
  explicit server(pfi::lang::shared_ptr<storage::storage_base>&,
		  const server_argv&,
                  const std::string& base_path = "/tmp");

  virtual ~server();

  // msgpack only
  result<std::string> get_storage(int);
  diffv get_diff(int);
  int put_diff(storage::features3_t v);

  //should be same in jubakeeper
  result<int> set_config(std::string, classifier::config_data);
  result<classifier::config_data> get_config(std::string);
  result<int> train(std::string,std::vector<std::pair<std::string, datum> > data);
  result<std::vector<estimate_results> > classify(std::string,std::vector<datum> data);
  result<int> save(std::string, std::string, std::string);
  result<int> load(std::string, std::string, std::string);
  result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > get_status(std::string);

  // internal use only
  void mix(const std::vector<std::pair<std::string, int> >&);
  void bind_all_methods(mprpc_server&, const std::string& host, int port);

private:

  config_data config_;
  pfi::lang::shared_ptr<datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<classifier_base> classifier_;
  pfi::lang::shared_ptr<storage::storage_base> storage_;
};

void mix_parameter(diffv& lhs, const diffv& rhs);

} // namespace classifier
} // namespace jubatus
