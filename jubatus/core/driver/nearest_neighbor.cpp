#include "nearest_neighbor.hpp"

namespace jubatus {
namespace core {
namespace driver {

nearest_neighbor::nearest_neighbor(
  const pfi::lang::shared_ptr<core::nearest_neighbor::nearest_neighbor_base>& nn,
  table::column_table* table,
  const pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter>& converter
) {
  mixable_table_.set_model(mixable_versioned_table::model_ptr(table));
  mixable_holder_.reset(new framework::mixable_holder);
  mixable_holder_->register_mixable(&mixable_table_);
  converter_ = converter;
  nn_ = nn;
}

void nearest_neighbor::set_row(const std::string& id, const fv_converter::datum& datum) {
  sfv_t v;
  converter_->convert(datum, v);
  nn_->set_row(id, v);
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::neighbor_row_from_id(const std::string& id, size_t size) {
  std::vector<std::pair<std::string, float> > ret;
  nn_->neighbor_row(id, ret, size);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::neighbor_row_from_data(const fv_converter::datum& datum, size_t size) {
  sfv_t v;
  converter_->convert(datum, v);
  std::vector<std::pair<std::string, float> > ret;
  nn_->neighbor_row(v, ret, size);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::similar_row(const std::string& id, size_t ret_num) {
  std::vector<std::pair<std::string, float> > ret;
  nn_->similar_row(id, ret, ret_num);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::similar_row(const core::fv_converter::datum& datum, size_t ret_num) {
  sfv_t v;
  converter_->convert(datum, v);
  std::vector<std::pair<std::string, float> > ret;
  nn_->similar_row(v, ret, ret_num);
  return ret;
}

} // driver
} // core
} // jubatus
