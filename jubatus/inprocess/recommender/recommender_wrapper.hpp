/*
 * Jubatus In-Process Wrapper
 */

#ifndef JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_HPP_
#define JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_HPP_

#include "../../core/driver/recommender.hpp"

namespace jubatus {
namespace inprocess {
namespace recommender {

class recommender {
 public:
  recommender();
  void save(std::ostream& os);
  void load(std::istream& is);
  pfi::lang::shared_ptr<jubatus::core::driver::recommender> get_driver();

 private:
  pfi::lang::shared_ptr<jubatus::core::driver::recommender> driver_;
};

}  // namespace recommender
}  // namespace inprocess
}  // namespace jubatus

#endif  // JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_HPP_
