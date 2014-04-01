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

#ifndef JUBATUS_CORE_BURST_BURST_STORAGE_BASE_HPP_
#define JUBATUS_CORE_BURST_BURST_STORAGE_BASE_HPP_

#include <string>
#include <vector>
#include <map>

#include "jubatus/server/server/burst_types.hpp"
#include "jubatus/util/lang/noncopyable.h"
#include "burst_system_manager.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

/**
 * the base class of burst storage.
 * this class performs the following <br>
 * - transmit to burst_system_manager the api method. <br>
 * - notiry server add/delete event (multinode only)
 */
class burst_storage_base : jubatus::util::lang::noncopyable {
 public:
  virtual ~burst_storage_base() {}

  /**
   * setter.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param result_window_rotate_size
   *        the number of calculated windows to keep
   */
  void set(int window_batch_size,
           int batch_interval,
           int max_reuse_batch_num,
           double costcut_threshold,
           int result_window_rotate_size);

  bool add_document(const std::string& str, double pos);

  bool add_keyword(const std::string& keyword_txt,
                   double scaling_param,
                   double gamma);

  bool remove_keyword(const std::string& keyword_txt);

  bool remove_all_keywords();

  std::vector<st_keyword> get_all_keywords() const;

  st_window get_result(const std::string& keyword_txt) const;

  st_window get_result_at(
      const std::string& keyword_txt, double pos) const;

  std::map<std::string, st_window> get_all_bursted_results() const;

  std::map<std::string, st_window>
      get_all_bursted_results_at(double pos) const;

  /**
   * the function to return whether or not to charge the local server.
   * @return charge the local server -> true,
   *         not charge -> false
   */
  virtual bool is_my_keyword(const std::string& keyword_txt) const = 0;

  jubatus::util::lang::shared_ptr<burst::system::burst_system_manager>
      get_burst_system_manager() const {
    return burst_system_manager_;
  }

 protected:
  burst_storage_base() {
  }

  jubatus::util::lang::shared_ptr<
      burst::system::burst_system_manager> burst_system_manager_;
};

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_BURST_STORAGE_BASE_HPP_
