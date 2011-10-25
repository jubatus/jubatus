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
#include <pficommon/network/mprpc.h>
#include <vector>
#include <map>

#include "../common/rpc_util.hpp"
#include "../server/classifier_rpc.hpp"

namespace jubatus{

MPRPC_PROC(lookup, result<std::vector<connection_info> >(std::string));
MPRPC_PROC(lookup_cht, result<std::vector<connection_info> >(std::string, std::string));

// classifier_serv must have same API as these
MPRPC_PROC(set_config, result<int>(std::string, classifier::config_data));
MPRPC_PROC(get_config, result<classifier::config_data>(std::string));
MPRPC_PROC(train, result<int>(std::string, std::vector<std::pair<std::string, datum> >))

MPRPC_PROC(classify, result<std::vector<classifier::estimate_results> >(std::string, std::vector<datum>));
MPRPC_PROC(save, result<int>(std::string, std::string, std::string));
MPRPC_PROC(load, result<int>(std::string, std::string, std::string));

MPRPC_PROC(get_status, result<std::map<std::pair<std::string,int>, std::map<std::string,std::string> > >(std::string));

MPRPC_GEN(1,
	  jubakeeper,
	  lookup, lookup_cht,
          set_config, get_config,
          train, classify, save, load, get_status);

}//jubatus
