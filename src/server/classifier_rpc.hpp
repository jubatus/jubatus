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
#include <map>

#include <pficommon/text/json.h>
#include <pficommon/network/mprpc.h>
#include <msgpack.hpp>

#include "../common/rpc_util.hpp"

#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"
#include "diffv.hpp"

namespace jubatus {
namespace classifier {

struct config_data
{
  config_data(const std::string& m,
              const jubatus::converter_config& c):
    method(m),converter(c)
  {};

  config_data() {};
  
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);

  template <class Archiver>
  void serialize(Archiver &ar) {
    ar
      & MEMBER(method)
      & MEMBER(converter);
  }
};

struct estimate_result
{
  estimate_result(const std::string &label,
                  double prob)
    : label_(label), prob_(prob) {}
  estimate_result() {}

  std::string label_;
  double prob_;

  MSGPACK_DEFINE(label_, prob_);

  template <class Archiver>
  void serialize(Archiver &ar) {
    ar 
      & NAMED_MEMBER("label", label_)
      & NAMED_MEMBER("prob", prob_);
  }
};

typedef std::vector<estimate_result> estimate_results;

MPRPC_PROC(get_storage, result<std::string>(int));

MPRPC_PROC(get_diff, diffv(int));
MPRPC_PROC(put_diff, int(storage::features3_t));

// jubakeeper must have same API as these
MPRPC_PROC(set_config, result<int>(std::string,config_data));
MPRPC_PROC(get_config, result<config_data>(std::string));
MPRPC_PROC(train, result<int>(std::string, std::vector<std::pair<std::string, datum> >));
MPRPC_PROC(classify, result<std::vector<classifier::estimate_results> >(std::string, std::vector<datum>));
MPRPC_PROC(save, result<int>(std::string, std::string, std::string));
MPRPC_PROC(load, result<int>(std::string, std::string, std::string));

MPRPC_PROC(get_status, result<std::map<std::pair<std::string,int>, std::map<std::string,std::string> > >(std::string));

MPRPC_GEN(1,
          mprpc,
	  get_storage, get_diff, put_diff,
          set_config, get_config,
          train, classify, save, load, get_status);

} // namespace classifier
} // namespace jubatus
