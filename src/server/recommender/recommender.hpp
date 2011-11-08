#pragma once

#include <vector>
#include <msgpack.hpp>
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include "../../common/type.hpp"
#include "recommender_type.hpp"

#include "anchor_finder_base.hpp"
#include "anchor_finder_factory.hpp"
#include "similarity_factory.hpp"
#include "../../server/recommender_rpc.hpp"

namespace jubatus {
namespace recommender {

class recommender{
public:
  recommender(const recommender_data&);
  recommender();
  ~recommender();

  void similar_row(const std::string& id, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void complete_row(const std::string& id, sfv_t& ret) const;
  void complete_row(const sfv_t& query, sfv_t& ret) const;
  void decode_row(const std::string& id, sfv_t& ret) const; 
  void get_all_rows(pfi::data::unordered_map<std::string, sfv_t>& ret_rows) const; 
  void swap(recommender& r);
  size_t anchor_num() const;
  // get_status();

  void convert_from_sfv_to_sfvi(const sfv_t& sfv, sfvi_t& sfvi) const;  

  recommender_data make_recommender_data();

private:
  friend class recommender_builder;
  pfi::lang::shared_ptr<anchor_finder_base> anchor_finder_;
  std::vector<sfvi_t> anchors_;
  pfi::data::unordered_map<std::string, data2anchors_t> data2anchors_;
  std::vector<anchor2data_t> anchor2data_;
  key_manager feature2id_;
};

} // namespace recommender
} // namespace jubatus
