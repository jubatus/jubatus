#ifndef JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_
#define JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_

#include <stdint.h>
#include <string>

#include <pficommon/lang/shared_ptr.h>

#include "../framework/mixable.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../nearest_neighbor/nearest_neighbor_base.hpp"
#include "../table/column/column_table.hpp"
#include "mixable_versioned_table.hpp"

namespace jubatus {
namespace core {
namespace driver {

class nearest_neighbor {
 public:
  // constructor
    nearest_neighbor(const pfi::lang::shared_ptr<jubatus::core::nearest_neighbor::nearest_neighbor_base>& nn, table::column_table* table, const pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter>& converter);

  // destructor

  const table::column_table* get_table() const {
    return nn_->get_table();
  }

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  void set_row(const std::string& id, const fv_converter::datum& datum);
  std::vector<std::pair<std::string, float> >
  neighbor_row_from_id(const std::string& id, size_t size);
  std::vector<std::pair<std::string, float> >
  neighbor_row_from_data(const fv_converter::datum& datum, size_t size);
  std::vector<std::pair<std::string, float> >
  similar_row(const std::string& id, size_t ret_num);
  std::vector<std::pair<std::string, float> >
  similar_row(const core::fv_converter::datum& datum, size_t ret_num);


 private:
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;
  // std::string my_id_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<jubatus::core::nearest_neighbor::nearest_neighbor_base> nn_;
  mixable_versioned_table mixable_table_;
};

} // namespace driver
} // namespace core
} // namespace jubatus


#endif // JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_
