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

#ifndef JUBATUS_CORE_FV_CONVERTER_DATUM_TO_FV_CONVERTER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_DATUM_TO_FV_CONVERTER_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/scoped_ptr.h>
#include "../common/type.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

enum frequency_weight_type {
  FREQ_BINARY,
  TERM_FREQUENCY,
  LOG_TERM_FREQUENCY
};

enum term_weight_type {
  TERM_BINARY,
  IDF,
  WITH_WEIGHT_FILE
};

struct splitter_weight_type {
  frequency_weight_type freq_weight_type_;
  term_weight_type term_weight_type_;

  splitter_weight_type(
      frequency_weight_type freq_weight_type,
      term_weight_type term_weight_type)
      : freq_weight_type_(freq_weight_type),
        term_weight_type_(term_weight_type) {
  }
};

struct datum;
class datum_to_fv_converter_impl;
class word_splitter;
class key_matcher;
class num_feature;
class string_filter;
class num_filter;
class weight_manager;

class datum_to_fv_converter {
 public:
  datum_to_fv_converter();

  ~datum_to_fv_converter();

  void convert(const datum& datum, sfv_t& ret_fv) const;

  void convert_and_update_weight(const datum& datum, sfv_t& ret_fv);

  void clear_rules();

  void register_string_filter(
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<string_filter> filter,
      const std::string& suffix);

  void register_num_filter(
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<num_filter> filter,
      const std::string& suffix);

  void register_string_rule(
      const std::string& name,
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<word_splitter> splitter,
      const std::vector<splitter_weight_type>& weights);

  void register_num_rule(
      const std::string& name,
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<num_feature> feature_func);

  void add_weight(const std::string& key, float weight);

  void revert_feature(
      const std::string& feature,
      std::pair<std::string, std::string>& expect) const;

  void set_hash_max_size(uint64_t hash_max_size);

  void set_weight_manager(pfi::lang::shared_ptr<weight_manager> wm);

 private:
  pfi::lang::scoped_ptr<datum_to_fv_converter_impl> pimpl_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_DATUM_TO_FV_CONVERTER_HPP_
