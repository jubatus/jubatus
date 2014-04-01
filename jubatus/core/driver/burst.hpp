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

#ifndef JUBATUS_CORE_DRIVER_BURST_HPP_
#define JUBATUS_CORE_DRIVER_BURST_HPP_

#include <string>
#include <utility>
#include <vector>
#include <map>
#include "jubatus/util/lang/shared_ptr.h"
#include "../framework/mixable.hpp"
#include "linear_function_mixer.hpp"

#include "jubatus/server/framework/server_util.hpp"
#include "jubatus/server/server/burst_types.hpp"
#include "../common/jsonconfig.hpp"
#include "../burst/mixable_burst.hpp"
#include "../burst/burst_storage_base.hpp"
#include "../burst/burst_local_storage.hpp"
#include "../burst/burst_distributed_storage.hpp"


namespace jubatus {
namespace core {
namespace driver {

class burst {
 public:
  typedef std::map<std::string, std::string> status_t;

  burst(const jubatus::core::common::jsonconfig::config& param,
        const jubatus::server::framework::server_argv& a);

  ~burst();

  jubatus::util::lang::shared_ptr<core::burst::storage::burst_storage_base>
  static create_burst_storage(
      const jubatus::core::common::jsonconfig::config& param,
      const jubatus::server::framework::server_argv& a);

  jubatus::util::lang::shared_ptr<framework::mixable_holder>
  get_mixable_holder() const {
    return mixable_holder_;
  }

  bool add_document(double pos, const fv_converter::datum& txt);

  st_window get_result(const std::string& keyword_txt) const;

  st_window get_result_at(const std::string& keyword_txt,
                                const double& pos) const;

  std::map<std::string, st_window> get_all_bursted_results() const;

  std::map<std::string, st_window>
  get_all_bursted_results_at(const double& pos) const;

  std::vector<st_keyword> get_all_keywords() const;

  bool add_keyword(const st_keyword& keyword);

  bool remove_keyword(const std::string& keyword_txt);

  bool remove_all_keywords();

  void get_status(std::map<std::string, std::string>& status) const;

  // void clear();

 private:
  jubatus::util::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  jubatus::util::lang::shared_ptr<
      jubatus::core::burst::storage::burst_storage_base> burst_storage_;

  jubatus::util::lang::shared_ptr<
      jubatus::core::burst::mixable_burst> mixable_burst_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_BURST_HPP_
