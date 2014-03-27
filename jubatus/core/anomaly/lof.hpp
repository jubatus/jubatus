// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_ANOMALY_LOF_HPP_
#define JUBATUS_CORE_ANOMALY_LOF_HPP_

#include <string>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/text/json.h"
#include "../recommender/recommender_base.hpp"
#include "anomaly_base.hpp"
#include "lof_storage.hpp"

namespace jubatus {
namespace core {
namespace anomaly {

class lof : public anomaly_base {
 public:
  lof();
  explicit lof(
      const lof_storage::config& config,
      jubatus::util::lang::shared_ptr<core::recommender::recommender_base>
          nn_engine);
  ~lof();

  // return anomaly score of query
  virtual float calc_anomaly_score(const common::sfv_t& query) const;
  virtual float calc_anomaly_score(const std::string& id) const;

  virtual void clear();
  virtual void clear_row(const std::string& id);
  virtual void update_row(const std::string& id, const sfv_diff_t& diff);
  virtual void set_row(const std::string& id, const common::sfv_t& sfv);

  virtual void get_all_row_ids(std::vector<std::string>& ids) const;
  virtual std::string type() const;
  virtual void register_mixables_to_holder(framework::mixable_holder& holder)
      const;
  bool is_updatable() const;

 private:
  jubatus::util::lang::shared_ptr<mixable_lof_storage> mixable_storage_;
  jubatus::util::lang::shared_ptr<recommender::recommender_base> nn_engine_;
};

}  //  namespace anomaly
}  // core
}  //  namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_LOF_HPP_
