#include <map>
#include <string>
#include "word_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

extern "C" {
word_splitter* create(const std::map<std::string, std::string>& params) {
  return 0;
}
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
