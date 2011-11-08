#pragma once

#include <string>

namespace jubatus {
namespace recommender {

class similarity_base;

class similarity_factory {
public:
  static similarity_base* create_similarity(const std::string& name);
};

} // namespace recommender
} // namespace jubatus
