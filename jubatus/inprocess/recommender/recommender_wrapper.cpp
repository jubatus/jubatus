/**
 * Jubatus In-Process Wrapper for Recommender
 */

#include "recommender_wrapper.hpp"

#include "../../core/driver/recommender.hpp"
#include "../../core/recommender/recommender_factory.hpp"
#include "../../core/fv_converter/datum.hpp"
#include "../../core/fv_converter/datum_to_fv_converter.hpp"
#include "../../core/fv_converter/converter_config.hpp"
#include "../../core/common/type.hpp"

namespace jubatus {
namespace inprocess {
namespace recommender {

recommender::recommender() {
  // Create defualt config
  jubatus::core::fv_converter::converter_config config;
  jubatus::core::fv_converter::num_rule rule;
  rule.key = "*";
  rule.type = "num";
  config.num_rules.push_back(rule);

  initialize(config);
}

recommender::recommender(
    jubatus::core::fv_converter::converter_config& config) {
  initialize(config);
}

void recommender::initialize(
    jubatus::core::fv_converter::converter_config& config) {
  pfi::lang::shared_ptr<jubatus::core::fv_converter::datum_to_fv_converter>
      converter(new jubatus::core::fv_converter::datum_to_fv_converter);
  jubatus::core::fv_converter::initialize_converter(config, *converter);

  driver_.reset(new jubatus::core::driver::recommender(
      jubatus::core::recommender::recommender_factory::create_recommender(),
      converter));
}

pfi::lang::shared_ptr<jubatus::core::driver::recommender>
    recommender::get_driver() {
  return driver_;
}

void recommender::save(std::ostream& os) {
  jubatus::core::framework::mixable_holder::mixable_list mixables =
      driver_->get_mixable_holder()->get_mixables();
  for (size_t i = 0; i < mixables.size(); ++i) {
    mixables[i]->save(os);
  }
}

void recommender::load(std::istream& is) {
  jubatus::core::framework::mixable_holder::mixable_list mixables =
      driver_->get_mixable_holder()->get_mixables();
  for (size_t i = 0; i < mixables.size(); ++i) {
    mixables[i]->clear();
    mixables[i]->load(is);
  }
}

}  // namespace recommender
}  // namespace inprocess
}  // namespace jubatus
