#include "similarity_factory.hpp"
#include "similarity.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

similarity_base* similarity_factory::create_similarity(const std::string& name){
  if (name == "cos"){
    return static_cast<similarity_base*>(new similarity_cos);
  } else if (name == "euclid"){
    return static_cast<similarity_base*>(new similarity_euclid);
  } else {
    return NULL;
  }
}

} // namespace recommender
} // namespace jubatus
