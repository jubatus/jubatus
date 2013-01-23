#include <gtest/gtest.h>
#include <cmath>

#include "keyword_weights.hpp"
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;

TEST(keyword_weights, trivial) {
  keyword_weights m, m2;
  {
    sfv_t fv;

    m.update_document_frequency(fv);

    fv.push_back(make_pair("key1", 1.0));
    fv.push_back(make_pair("key2", 1.0));
    m.update_document_frequency(fv);

    m.add_weight("key3", 2.0);

    EXPECT_EQ(2u, m.get_document_count());
    EXPECT_EQ(1u, m.get_document_frequency("key1"));
    EXPECT_EQ(0u, m.get_document_frequency("unknown"));
  }

  {
    sfv_t fv;
    m2.update_document_frequency(fv);

    fv.push_back(make_pair("key1", 1.0));
    fv.push_back(make_pair("key2", 1.0));
    m2.update_document_frequency(fv);

    m2.add_weight("key3", 3.0);

    m.merge(m2);

    EXPECT_EQ(4u, m.get_document_count());
    EXPECT_EQ(2u, m.get_document_frequency("key1"));
    EXPECT_EQ(3.0, m.get_user_weight("key3"));
    EXPECT_EQ(0u, m.get_document_frequency("unknown"));
  }

  {
    m.clear();
    EXPECT_EQ(0u, m.get_document_count());
    EXPECT_EQ(0u, m.get_document_frequency("key1"));
    EXPECT_EQ(0.0, m.get_user_weight("key3"));
  }
}

}
}
