#include "anchor_builder_base.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

void anchor_builder_base::build(const pfi::data::unordered_map<string, sfvi_t> originals, 
                                size_t anchor_num,
                                vector<string>& anchors){
  anchors.clear();
  vector<string> ids;
  vector<sfvi_t> sfvis;
  for (pfi::data::unordered_map<string, sfvi_t>::const_iterator it = originals.begin(); it != originals.end(); ++it){
    ids.push_back(it->first);
    sfvis.push_back(it->second);
  }
  vector<size_t> anchor_inds;
  build(sfvis, anchor_num, anchor_inds);
  for (size_t i = 0; i < anchor_inds.size(); ++i){
    anchors.push_back(ids[anchor_inds[i]]);
  }
}


} // namespace recommender
} // namespace jubatus
