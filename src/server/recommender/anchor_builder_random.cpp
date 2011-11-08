#include <algorithm>

#include "anchor_builder_random.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

void anchor_builder_random::build(const vector<sfvi_t>& sfvs,
                                  size_t anchor_num,
                                  vector<size_t>& anchors){
  anchors.clear();
  size_t choosed_num = min(anchor_num, sfvs.size());
  for (size_t i = 0; i < sfvs.size(); ++i){
    anchors.push_back(i);
  }
  random_shuffle(anchors.begin(), anchors.end());
  anchors.resize(choosed_num);
}

} // namespace recommender
} // namespace jubatus

