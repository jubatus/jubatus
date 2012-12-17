// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "recommender_factory.hpp"
#include "recommender.hpp"
#include "../common/exception.hpp"
#include "../common/config_util.hpp"
#include "../storage/norm_factory.hpp"

using namespace std;
using pfi::text::json::json;

namespace jubatus {
namespace recommender {

recommender_base* create_recommender(const string& name,
                                     const json param) {
  // TODO: use param for each method
  if (name == "inverted_index"){
    return new inverted_index;
  } else if (name == "minhash"){
    return new minhash;
  } else if (name == "lsh"){
    return new lsh;
  } else if (name == "euclid_lsh") {
    euclid_lsh::euclid_lsh_config config;
    config.lsh_num = get_param<int>(param, "lsh_num", euclid_lsh::DEFAULT_LSH_NUM);
    config.table_num = get_param<int>(param, "table_num", euclid_lsh::DEFAULT_TABLE_NUM);
    config.seed = get_param<int>(param, "seed", euclid_lsh::DEFAULT_SEED);
    config.bin_width = get_param<int>(param, "bin_width", euclid_lsh::DEFAULT_BIN_WIDTH);
    config.probe_num = get_param<int>(param, "probe_num", euclid_lsh::DEFAULT_NUM_PROBE);
    config.retain_projection = get_param<bool>(param, "retain_projection", euclid_lsh::DEFAULT_RETAIN_PROJECTION);
    return new euclid_lsh(config);
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}
}



