#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include "recommender_factory.hpp"
#include "recommender_mock.hpp"
#include "recommender_mock_util.hpp"

using namespace std;
using pfi::lang::scoped_ptr;

namespace jubatus {
namespace recommender {

TEST(recommender_mock, similar_row_by_id) {
  recommender_mock r;
  r.update_row("r1", make_sfv("f1:1 f2:1"));

  const vector<pair<string, float> > expect = make_ids("r2:2 r3:1");
  r.set_similar_relation("r1", expect);

  vector<pair<string, float> > ids;
  r.similar_row("r1", ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, neighbor_row_by_id) {
  recommender_mock r;
  r.update_row("r1", make_sfv("f1:1 f2:1"));

  const vector<pair<string, float> > expect = make_ids("r2:1 r3:2");
  r.set_neighbor_relation("r1", expect);

  vector<pair<string, float> > ids;
  r.neighbor_row("r1", ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, similar_row_by_query) {
  recommender_mock r;

  const sfv_t query = make_sfv("f1:1 f2:1");
  const vector<pair<string, float> > expect = make_ids("r2:2 r3:1");
  r.set_similar_relation(query, expect);

  vector<pair<string, float> > ids;
  r.similar_row(query, ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, neighbor_row_by_query) {
  recommender_mock r;

  const sfv_t query = make_sfv("f1:1 f2:1");
  const vector<pair<string, float> > expect = make_ids("r2:1 r3:2");
  r.set_neighbor_relation(query, expect);

  vector<pair<string, float> > ids;
  r.neighbor_row(query, ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, name) {
  scoped_ptr<recommender_base> r(create_recommender("mock"));
  EXPECT_EQ("recommender_mock", r->type());
}

}
}
