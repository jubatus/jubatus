#pragma once

#include <pficommon/data/unordered_map.h>
#include "../../common/key_manager.hpp"
#include "../../common/type.hpp"


namespace jubatus {
namespace recommender {

class recommender;
class similarity_base;
class anchor_finder_base;
class anchor_builder_base;

typedef sfv_t sfv_diff_t;
typedef std::vector<std::pair<std::string,sfv_t> > recommender_diff_t;
  //typedef pfi::data::unordered_map<std::string, sfv_t> recommender_diff_t;

class recommender_builder{
public:
  recommender_builder(const std::string& similarity_name,
		      const std::string& anchor_finder_name,
		      const std::string& anchor_builder_name) :
    similarity_name_(similarity_name),
    anchor_finder_name_(anchor_finder_name),
    anchor_builder_name_(anchor_builder_name) {}

  ~recommender_builder();

  //void add_row(const std::string& id, const sfv_t& sfv);
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);

  void get_diff(recommender_diff_t& ret) const;

  void build(const recommender& base,
             size_t all_anchor_num, 
             size_t anchor_num_per_data,
             recommender& r);

private:
  std::string similarity_name_;
  std::string anchor_finder_name_;
  std::string anchor_builder_name_;
  key_manager feature2id_;

  void canonalize_originals();
  pfi::data::unordered_map<std::string, sfvi_t> originals_;
};

} // namespace recommender
} // namespace jubatus
