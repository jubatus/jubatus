#include <gtest/gtest.h>
#include <cmath>

#include "keyword_weights.hpp"
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;

TEST(keyword_weights, trivial) {
  keyword_weights m;
  sfv_t fv;

  m.update_document_frequency(fv);

  fv.push_back(make_pair("/title$this@space#bin/bin", 1.0));
  fv.push_back(make_pair("/title$this@space#bin/idf", 1.0));
  fv.push_back(make_pair("/age@bin", 1.0));
  m.update_document_frequency(fv);

  EXPECT_EQ(1u, m.get_document_frequency("/title$this@space#bin/bin"));
  EXPECT_EQ(0u, m.get_document_frequency("unknown"));
}

}
}
