#include <gtest/gtest.h>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>

#include "recommender_serv.hpp"
#include "../fv_converter/converter_config.hpp"

using namespace std;

using pfi::lang::lexical_cast;
using pfi::text::json::to_json;

namespace jubatus {
namespace server {

datum make_datum(const string& s) {
  datum d;
  d.string_values.push_back(make_pair("v", s));
  return d;
}

TEST(recommender_serv, feature_vector_weight) {
  framework::server_argv arg;
  recommender_serv s(arg);
  config_data conf;
  conf.method = "inverted_index";
  fv_converter::converter_config config;
  fv_converter::string_rule r = {"*", "space", "bin", "idf"};
  config.string_rules.push_back(r);
  conf.converter = lexical_cast<string>(to_json(config));
  s.set_config(conf);

  // v1 = (1, 1, 0, 0)
  // v2 = (1, 0, 1, 1)
  // v3 = (1, 0, 1, 1)

  s.update_row("2", make_datum("1 3 4"));
  s.update_row("3", make_datum("1 3 4"));
  s.update_row("4", make_datum("1 3 4"));
  s.update_row("1", make_datum("1 2"));

  similar_result res = s.similar_row_from_data(make_datum("2 3 4"), 1);
  ASSERT_EQ(1u, res.size());
  EXPECT_EQ("1", res[0].first);
  cout << res[0].second << endl;
}

}
}
