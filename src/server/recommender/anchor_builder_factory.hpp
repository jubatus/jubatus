#pragma once

#include <string>

namespace jubatus {
namespace recommender {

class anchor_builder_base;

class anchor_builder_factory {
public:
  static anchor_builder_base* create_anchor_builder(const std::string& name);
};

} // namespace recommender
} // namespace jubatus
