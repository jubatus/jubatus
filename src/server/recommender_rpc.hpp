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

#include <pficommon/network/mprpc.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/serialization.h>

#include <msgpack.hpp>

#include "../common/rpc_util.hpp"

#include "../fv_converter/datum.hpp"
#include "../storage/local_storage_mixture.hpp"
#include "../fv_converter/converter_config.hpp"
#include "diffv.hpp"
#include "../common/type.hpp"
#include "../recommender/recommender_type.hpp"

namespace jubatus {
namespace recommender {

struct config_data
{
  explicit config_data(const jubatus::converter_config& c):
    converter(c)
  {};

  config_data() {};
  
  MSGPACK_DEFINE(converter, similarity_name, anchor_finder_name, anchor_builder_name, all_anchor_num, anchor_num_per_data);

  template <class Archiver>
  void serialize(Archiver &ar) {
    ar
      & MEMBER(converter)
      & MEMBER(similarity_name)
      & MEMBER(anchor_finder_name)
      & MEMBER(anchor_builder_name)
      & MEMBER(all_anchor_num)
      & MEMBER(anchor_num_per_data);
  }

  std::string similarity_name;
  std::string anchor_finder_name;
  std::string anchor_builder_name;

  size_t all_anchor_num;
  size_t anchor_num_per_data;

  jubatus::converter_config converter;
};

struct recommender_data
{
  recommender_data(){};
  ~recommender_data(){};

  MSGPACK_DEFINE(similarity_name, anchor_finder_name, serialized_anchor_finder,
                 anchors, id2key, data2anchors, anchor2data);
  //  template <typename MSGPACK_OBJECT>
  //  void msgpack_object(MSGPACK_OBJECT* o, msgpack::zone* z)const{}

  std::string similarity_name;
  std::string anchor_finder_name;
  std::string serialized_anchor_finder;
  std::vector<sfvi_t> anchors;
  std::vector<std::string> id2key;
  std::vector<std::pair<std::string, data2anchors_t> > data2anchors;
  std::vector<anchor2data_t> anchor2data;
};

// typedef std::vector<std::pair<std::string, datum> > raw_data;
typedef std::vector<std::pair<std::string, float> > similar_result;
typedef std::vector<std::pair<std::string, datum> > rows;

MPRPC_PROC(set_config, result<int>(std::string, config_data));
MPRPC_PROC(get_config, result<config_data>(std::string));

MPRPC_PROC(clear_row,  result<int>(std::string,std::vector<std::string>));
MPRPC_PROC(update_row, result<int>(std::string,rows));

MPRPC_PROC(build, result<int>(std::string));
MPRPC_PROC(clear, result<int>(std::string));
MPRPC_PROC(get_status, result<std::map<std::pair<std::string,int>, std::map<std::string, std::string> > >(std::string));

MPRPC_PROC(complete_row_from_id,   result<datum>(std::string,std::string));
MPRPC_PROC(complete_row_from_data, result<datum>(std::string,datum));
MPRPC_PROC(similar_row_from_id,    result<similar_result>(std::string,std::string, size_t));
MPRPC_PROC(similar_row_from_data,  result<similar_result>(std::string,datum, size_t));
MPRPC_PROC(decode_row,   result<datum>(std::string,std::string));
MPRPC_PROC(get_all_rows, result<rows>(std::string));

typedef std::vector<std::pair<std::string, sfv_t> >recommender_diff_t;
MPRPC_PROC(get_diff, recommender_diff_t(int));
MPRPC_PROC(put_diff, int(recommender_data));

MPRPC_GEN(1,
          mprpc,
          set_config, get_config,
          clear_row, update_row,
          build, clear,
          get_status,
          complete_row_from_id,
          complete_row_from_data,
          similar_row_from_id,
          similar_row_from_data,
          decode_row,
          get_all_rows,
          get_diff,
          put_diff
          )

} // namespace recommender
} // namespace jubatus
