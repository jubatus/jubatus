#include <stdexcept>
#include "regression_factory.hpp"
#include "regression.hpp"

namespace jubatus {

regression_base* regression_factory::create_regression(const std::string& name,
                                                       storage::storage_base* storage) {
  if (name == "online_svr") {
    return new online_svr(storage);
  } else {
    throw std::runtime_error("unknown regression type: " + name);
  }
}


}
