#include <pficommon/data/unordered_map.h>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <stdint.h>
#include <cassert>
#include "anchor_finder_base.hpp"
#include "anchor_finder_knndecent.hpp"
#include "similarity_base.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

namespace {
const float EPS = 1e-8;
} // namespace

bool sample_without_replacement(int n, int k, std::vector<int> &res) {
  res.clear();
  if (n<=0 || k<0 || k>n) return false;

  // shuffle random numbers lazily
  pfi::data::unordered_map<int,int> mm; // mapping from position to value
  for (int i=0;i<k;++i) mm[i]=i;
  for (int i=0;i<k;++i) {
    int j=i+rand()%(n-i);
    if (!mm.count(j)) {
      mm[j]=j;
    }
    swap(mm[i], mm[j]);
  }
  res.resize(k);
  for (pfi::data::unordered_map<int,int>::iterator it=mm.begin();it!=mm.end();++it) {
    if (0 <= it->first && it->first < k) {
      res[it->first] = it->second;
    }
  }
  return true;
}

anchor_finder_knndecent::anchor_finder_knndecent(similarity_base *dis)
  : anchor_finder_base(dis)
{
  k_ = 10;
  step_num_ = 100;
}

void anchor_finder_knndecent::set_k(size_t k){
  k_ = k;
}

void anchor_finder_knndecent::set_step_num(size_t step_num){
  step_num_ = step_num;
}

void anchor_finder_knndecent::init_graph(const std::vector<sfvi_t> &anchors, knngraph_t &g) {
  g.resize(anchors.size());
  for (size_t ix = 0; ix < anchors.size(); ++ix) {
    vector<int> samples;
    sample_without_replacement(anchors.size(), bag_size_ + 1, samples);
    for (size_t j = 0; j < samples.size(); ++j) {
      size_t jx = samples[j];
      if (jx == ix) continue;
      float dist = (*dis_)(anchors[ix], anchors[jx]);
      g[ix].push_back(make_pair(dist, jx));
    }
    sort(g[ix].rbegin(), g[ix].rend());
  }
}

bool anchor_finder_knndecent::update_index_at_bag(const std::vector<sfvi_t> &anchors, const bag_t &bag) {
  bool updated = false;

  for (size_t i = 0; i < bag.size(); ++i) {
    size_t ix = bag[i].second;
    bag_t& ix_bag = graph_[ix];
    float farthest_dist = ix_bag.back().first;
    for (size_t j = 0; j < bag.size(); ++j) {
      if (i == j) continue;
      size_t jx = bag[j].second;
      float dist = (*dis_)(anchors[ix], anchors[jx]);
      ix_bag.push_back(make_pair(dist, jx));
    }
    sort(ix_bag.rbegin(), ix_bag.rend());
    ix_bag.erase(unique(ix_bag.begin(), ix_bag.end()), ix_bag.end());
    ix_bag.erase(ix_bag.begin() + bag_size_, ix_bag.end());
    float new_farthest_dist = graph_[ix].back().first;
    updated |= new_farthest_dist < farthest_dist - EPS;
  }
  return updated;
}

void anchor_finder_knndecent::build_index(const vector<sfvi_t> &anchors){
  if (anchors.size() == 0) return;
  bag_size_ = min(k_, anchors.size() - 1);
  init_graph(anchors, graph_);

  size_t update_num = anchors.size();
  for (size_t step = 0; step < step_num_; ++step) {
    update_num = 0;
    for (size_t i = 0; i < anchors.size(); ++i) {
      update_num += update_index_at_bag(anchors, graph_[i]);
    }
    if (update_num == 0) break; 
  }
}

bool anchor_finder_knndecent::update_bag(const sfvi_t &fv, bag_t &bag, const vector<sfvi_t> &anchors) const {
  int orig_size = bag.size();
  float farthest_dist = bag.back().first;
  for (size_t i = 0; i < bag.size(); ++i) {
    size_t ix = bag[i].second;
    for (size_t j = 0; j < graph_[ix].size(); ++j) {
      size_t jx = graph_[ix][j].second;
      float dist = (*dis_)(fv, anchors[jx]);
      bag.push_back(make_pair(dist, jx));
    }
    sort(bag.rbegin(), bag.rend());
    bag.erase(unique(bag.begin(), bag.end()), bag.end());
    bag.erase(bag.begin() + orig_size, bag.end());
  }
  float updated_farthest_dist = bag.back().first;
  return updated_farthest_dist < farthest_dist - EPS;
}

void anchor_finder_knndecent::find(const sfvi_t &fv, const std::vector<sfvi_t> &anchors, 
                                   size_t ret_num, data2anchors_t &ret) const {
  ret_num = min(ret_num, anchors.size());
  ret.clear();
  if (ret_num == 0) return;

  bag_t bag;

  // init bag 
  vector<int> samples;
  sample_without_replacement(anchors.size(), ret_num, samples);
  for (size_t i = 0; i < samples.size(); ++i) {
    size_t ix = samples[i];
    float dist = (*dis_)(anchors[ix], fv);
    bag.push_back(make_pair(dist, ix));
  }
  sort(bag.rbegin(), bag.rend());

  // update bag
  bool updated = true;
  for (size_t step = 0; step < step_num_ && updated; ++ step) {
    updated = update_bag(fv, bag, anchors);
  }

  // return bag
  for (size_t i = 0; i < bag.size(); ++i) {
    ret.push_back(make_pair(bag[i].second, bag[i].first));
  }
}

std::string anchor_finder_knndecent::name() const {
  return "knndecent";
}

} // namespace recommender
} // namespace jubatus
