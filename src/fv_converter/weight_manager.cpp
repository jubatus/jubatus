#include <cmath>
#include "../common/type.hpp"
#include "weight_manager.hpp"
#include "datum_to_fv_converter.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;

weight_manager::weight_manager()
    : document_count_(),
      document_frequencies_(),
      weights_() {}

struct is_zero {
  bool operator()(const pair<string, float>& p) {
    return p.second == 0;
  }
};

void weight_manager::update_weight(sfv_t& fv) {
  ++document_count_;
  for (sfv_t::const_iterator it = fv.begin(); it != fv.end(); ++it) {
    ++document_frequencies_[it->first];
  }

  for (sfv_t::iterator it = fv.begin(); it != fv.end(); ++it) {
    double global_weight  = get_global_weight(it->first);
    it->second *= global_weight;
  }
  fv.erase(remove_if(fv.begin(), fv.end(), is_zero()), fv.end());
}


double weight_manager::get_global_weight(const string& key) const {
  size_t p = key.find_last_of('/');
  if (p == string::npos)
    return 1.0;
  string type = key.substr(p + 1);
  if (type == "bin") {
    return 1.0;
  } else if (type == "idf") {
    return log(static_cast<double>(document_count_)
               / document_frequencies_[key]);
  } else if (type == "weight") {
    p = key.find_last_of('#');
    if (p == string::npos)
      return 0;
    weight_t::const_iterator wit = weights_.find(key.substr(0, p));
    if (wit != weights_.end()) {
      return wit->second;
    } else {
      return 0;
    }
  } else {
    return 1;
  }
}

void weight_manager::add_weight(const std::string& key, float weight) {
  weights_[key] = weight;
}

}
}
