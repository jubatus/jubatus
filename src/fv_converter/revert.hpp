#pragma once

#include <string>
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

class datum;

void revert_feature(const sfv_t& fv,
                    datum& data);

bool revert_num_value(const std::pair<std::string, float>& feature,
                      std::pair<std::string, float>& num_value);

bool revert_string_value(const std::pair<std::string, float>& feature,
                         std::pair<std::string, std::string>& string_value);

}
}
