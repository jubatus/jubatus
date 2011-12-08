#include <gtest/gtest.h>

#include "recommender_base.hpp"
#include "../storage/recommender_storage.hpp"
#include "../storage/norm_none.hpp"

using namespace pfi::lang;

namespace jubatus {
namespace recommender {

using namespace std;

shared_ptr<storage::recommender_storage> make_storage() {
  shared_ptr<storage::norm_base> n(new storage::norm_none());
  return shared_ptr<storage::recommender_storage>(new storage::recommender_storage(n));
}

class recommender_impl : public recommender_base{
 public:
  recommender_impl()
      : recommender_base(make_storage()) {
    // make mock
    origs_->set("r1", "a1", 1.0);
    origs_->set("r1", "a2", 1.0);

    origs_->set("r2", "b1", 1.0);
    origs_->set("r2", "b2", 1.0);

    origs_->set("r3", "a1", 1.0);
    origs_->set("r3", "b1", 1.0);
  }

  void similar_row(const sfv_t& query, vector<pair<string, float> > & ids, size_t ret_num) const{
    ids.clear();
    ids.push_back(make_pair("r1", 2.0));
    ids.push_back(make_pair("r3", 1.0));
  }
  
  void clear(){}
  void clear_row(const string& id){}
  void update_row(const string& id, const sfv_diff_t& diff){
  }
};

TEST(recommender_base, complete_row) {
  recommender_impl r;
  sfv_t q;
  sfv_t ret;
  r.complete_row(q, ret);
  ASSERT_EQ(3u, ret.size());
  EXPECT_EQ("a1", ret[0].first);
  EXPECT_EQ("a2", ret[1].first);
  EXPECT_EQ("b1", ret[2].first);
}

TEST(recommender_base, get_all_row_ids) {
  vector<string> ids;
  recommender_impl r;
  r.get_all_row_ids(ids);
  ASSERT_EQ(3u, ids.size());
  sort(ids.begin(), ids.end());
  EXPECT_EQ("r1", ids[0]);
  EXPECT_EQ("r2", ids[1]);
  EXPECT_EQ("r3", ids[2]);
}

TEST(recommender_base, decode_row) {
  recommender_impl r;
  sfv_t v;
  r.decode_row("r1", v);
  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("a1", v[0].first);
  EXPECT_EQ(1.0,  v[0].second);
  EXPECT_EQ("a2", v[1].first);
  EXPECT_EQ(1.0, v[1].second);  

  r.decode_row("r", v);
  ASSERT_TRUE(v.empty());
}

}
}
