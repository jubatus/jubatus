#include "vector_util.hpp"
#include <algorithm>

namespace jubatus {

using namespace std;

void sort_and_merge(sfv_t& sfv){
  if (sfv.size() == 0) return;
  sort(sfv.begin(), sfv.end());
  sfv_t ret_sfv;
  const string* prev = &sfv[0].first;
  float val = sfv[0].second;
  for (size_t i = 1; i < sfv.size(); ++i){
    if (sfv[i].first == *prev){
      val += sfv[i].second;
    } else {
      ret_sfv.push_back(make_pair(*prev, val));
      prev = &sfv[i].first;
      val = sfv[i].second;
    }
  }
  ret_sfv.push_back(make_pair(*prev, val));
  sfv.swap(ret_sfv);
}

}
