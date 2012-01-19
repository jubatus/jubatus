#pragma once

#include "datum.hpp"
#include "counter.hpp"
#include <pficommon/data/unordered_map.h>
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

class weight_manager {
 public:
  weight_manager();
  
  void update_weight(sfv_t& fv);

  void add_weight(const std::string& key, float weight);

 private:
  double get_global_weight(const std::string& key) const;

  size_t document_count_;
  counter<std::string> document_frequencies_;
  typedef pfi::data::unordered_map<std::string, float> weight_t;
  weight_t weights_;

};

}
}
