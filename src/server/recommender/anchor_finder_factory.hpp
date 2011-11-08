#pragma once

#include <string>
#include <sstream>

namespace jubatus {
namespace recommender {

class anchor_finder_base;
class similarity_base;

class anchor_finder_factory {
public:
  static anchor_finder_base* create_anchor_finder(const std::string& name, similarity_base *dis);
  
  static void pack(std::stringstream& ss, const std::string& name, anchor_finder_base* anchor_finder);

  static anchor_finder_base* unpack(const std::string& serialized_anchor_finder,
                                    const std::string& similarity_name, const std::string& anchor_name);
};

} // namespace recommender
} // namespace jubatus
