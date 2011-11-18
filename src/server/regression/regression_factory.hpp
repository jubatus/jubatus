#pragma once

#include <string>

namespace jubatus {

class regression_base;

namespace storage {
class storage_base;
}

class regression_factory {
 public:
  regression_base* create_regression(const std::string& name,
                                     storage::storage_base* storage) const;
};

}
