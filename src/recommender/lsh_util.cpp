#include <cmath>
#include "lsh_util.hpp"
#include "../storage/bit_vector.hpp"

namespace jubatus {
namespace recommender {

using namespace std;
using jubatus::storage::bit_vector;
using pfi::data::unordered_map;

void generate_random_vector(size_t dim, vector<float>& ret) {
  // TODO use random number generator
  vector<float> v(dim);
  for (uint64_t i = 0; i < dim; ++i){
    float v1 = (float)(rand()+1) / ((float)RAND_MAX + 1);
    float v2 = (float)(rand()+1) / ((float)RAND_MAX + 1);
    float z = sqrt(-2.f * log(v1)) * cos(2.f * M_PI * v2);
    v[i] = z;
  }
  ret.swap(v);
}

void set_bit_vector(const std::vector<float>& vec,
                    bit_vector& bit_vec) {
  bit_vector bv;
  bv.resize_and_clear(vec.size());
  for (size_t i = 0; i < vec.size(); ++i){
    if (vec[i] >= 0.f){
      bv.set_bit(i);
    }
  }
  bit_vec.swap(bv);
}

void prod_invert_and_vector(
    const unordered_map<string, vector<float> >& matrix,
    const sfv_t& vec,
    size_t dim,
    vector<float>& ret) {
  vector<float> r(dim);
  for (size_t i = 0; i < vec.size(); ++i){
    const string& column = vec[i].first;
    float val = vec[i].second;
    unordered_map<string, vector<float> >::const_iterator it = matrix.find(column);
    if (it == matrix.end()){
      continue;
    }
    const vector<float>& v = it->second;

    // assert(v.size() == base_num_);
    for (size_t j = 0; j < v.size(); ++j){
      r[j] += v[j] * val;
    }  
  }

  ret.swap(r);
}



}
}
