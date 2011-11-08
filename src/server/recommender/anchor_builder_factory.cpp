#include "anchor_builder_factory.hpp"
#include "anchor_builder_random.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

anchor_builder_base* anchor_builder_factory::create_anchor_builder(const std::string& name){
  if (name == "random"){
    return static_cast<anchor_builder_base*>(new anchor_builder_random);
  } else {
    return NULL;
  }
}

} // namespace recommender
} // namespace jubatus
