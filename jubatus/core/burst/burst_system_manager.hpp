// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_BURST_BURST_SYSTEM_MANAGER_HPP_
#define JUBATUS_CORE_BURST_BURST_SYSTEM_MANAGER_HPP_

#include <vector>
#include <list>
#include <map>
#include <string>
#include <msgpack.hpp>

#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/server/server/burst_types.hpp"
#include "burst_system_builder.hpp"
#include "my_keyword_list.hpp"
#include "mix_data.hpp"
#include "../common/version.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * burst_system manage class
 */
class burst_system_manager {
 public:
  /**
   * constructor.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @param result_window_rotate_size
   *        the number of calculated windows to keep
   */
  burst_system_manager(const int32_t window_batch_size,
                       const int32_t batch_interval,
                       const double& costcut_threshold,
                       const int32_t max_reuse_batch_num,
                       const int32_t result_window_rotate_size);

  /**
   * see idl.
   */
  bool add_document(const std::string& str, const double& pos);

  /**
   * add keyword to burst_system_map
   * @param keyword_txt
   *        keyword
   * @param scaling_param
   *        Kleinberg paper's "scaling parameter"
   * @param gamma
   *        Kleinberg paper's "gamma"
   */
  bool add_keyword_to_burst_system_map(const std::string& keyword_txt,
                                       const double& scaling_param,
                                       const double& gamma);
  /**
   * add keyword to my_keyword_list
   * @param keyword_txt
   *        keyword
   */
  bool add_keyword_to_my_keyword_list(const std::string& keyword_txt);

  /**
   * see idl.
   */
  bool remove_keyword(const std::string& keyword_txt);

  /**
   * see idl.
   */
  bool remove_all_keywords();

  /**
   * update my_keyword_list
   * @param keywords
   *        all my_keywords
   */
  bool update_all_keywords(const std::list<std::string>& keywords);

  /**
   * get list of all my keywords text
   * @return list of all my keywords text
   */
  const std::list<std::string> get_my_keywords_txt() const;

  /**
   * see idl.
   */
  const std::vector<st_keyword> get_all_keywords() const;

  /**
   * get list of all added keywords
   * @return list of all added keywords
   */
  const std::vector<std::string> get_all_keywords_txt() const;

  /**
   * see idl.
   */
  const st_window
  get_result(const std::string& keyword_txt) const;

  /**
   * see idl.
   */
  const st_window
  get_result_at(const std::string& keyword_txt, const double& pos) const;

  /**
   * see idl.
   */
  const std::map<std::string, st_window>
  get_all_bursted_results() const;

  /**
   * see idl.
   */
  const std::map<std::string, st_window>
  get_all_bursted_results_at(const double& pos) const;

  /**
   * get diff data on mix.
   * @return the map of keyword and mix_data of the keyword.
   */
  std::map<std::string, mix_data> get_diff_impl() const;
  void get_diff(std::map<std::string, mix_data>& ret) const;

  /**
   * put diff data on mix.
   * @param diff the map of keyword and mix_data of the keyword.
   */
  bool put_diff_impl(const std::map<std::string, mix_data>& diff);

  bool set_mixed_and_clear_diff(const std::map<std::string, mix_data>& diff);

  void mix(const std::map<std::string, mix_data>& lhs,
           std::map<std::string, mix_data>& rhs) const;

  /**
   * pack model (unsupported)
   */
  void pack(msgpack::packer<msgpack::sbuffer>& packer) const;

  /**
   * unpack msgpack object into model (unsupported)
   */
  void unpack(msgpack::object o);

  void clear();

  storage::version get_version() const {
    return storage::version();
  }

 private:
  const st_window window_to_st_window(const window& w) const;

  std::map<std::string, burst_system*> burst_system_map_;

  burst_system_builder burst_system_builder_;

  jubatus::util::lang::shared_ptr<util::my_keyword_list> my_keyword_list_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus


#endif  // JUBATUS_CORE_BURST_BURST_SYSTEM_MANAGER_HPP_
