#include <stdexcept>
#include "regression_factory.hpp"
#include "regression.hpp"

namespace jubatus {

regression_base*
regression_factory::create_regression(const std::string& name,
                                      storage::storage_base* storage) const {
  if (name == "PA") {
    return new regression::PA(storage);
  } else {
    throw std::runtime_error("unknown regression type: " + name);
  }
}


}
