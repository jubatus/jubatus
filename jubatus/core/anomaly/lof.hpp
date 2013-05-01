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
#include <pficommon/text/json.h>
#include "anomaly_base.hpp"
#include "lof_storage.hpp"

namespace jubatus {
namespace core {
namespace anomaly {

class lof : public anomaly_base {
 public:
  lof();
  explicit lof(
      const storage::lof_storage::config& config,
      core::recommender::recommender_base* nn_engine);
  ~lof();

  // return anomaly score of query
  virtual float calc_anomaly_score(const sfv_t& query) const;
  virtual float calc_anomaly_score(const std::string& id) const;

  virtual void clear();
  virtual void clear_row(const std::string& id);
  virtual void update_row(const std::string& id, const sfv_diff_t& diff);
  virtual void get_all_row_ids(std::vector<std::string>& ids) const;

  virtual std::string type() const;
  virtual storage::anomaly_storage_base* get_storage();
  virtual const storage::anomaly_storage_base* get_const_storage() const;

  void save(std::ostream&);
  void load(std::istream&);

  // static float calc_distance(sfv_t& q1, sfv_t& q2);
  // static float calc_l2norm(sfv_t& q1, sfv_t& q2);

 private:
  virtual bool save_impl(std::ostream& os);
  virtual bool load_impl(std::istream& is);

  storage::lof_storage lof_index_;
};

}  //  namespace anomaly
}  // core
}  //  namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_LOF_HPP_
