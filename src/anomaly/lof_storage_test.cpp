#include <sstream>
#include <gtest/gtest.h>
#include <pficommon/lang/cast.h>
#include "lof_storage.hpp"

using namespace std;
using pfi::lang::lexical_cast;

namespace jubatus {
namespace storage {

namespace {

sfv_t make_sfv(const string& str) {
  istringstream iss(str);
  sfv_t sfv;

  size_t idx;
  float value;
  char colon;
  while (iss >> idx >> colon >> value) {
    sfv.push_back(make_pair(lexical_cast<string>(idx), value));
  }

  return sfv;
}

}

TEST(lof_storage, name) {
  lof_storage s;
  EXPECT_EQ("lof_storage", s.name());
}

TEST(lof_storage, get_all_row_ids) {
  lof_storage s;
  s.update_row("r1", make_sfv("1:1"));
  s.update_row("r2", make_sfv("2:1"));
  s.update_row("r4", make_sfv("1:-1"));
  s.update_row("r5", make_sfv("2:-1"));
  s.update_row("r6", make_sfv("3:-1"));

  s.remove_row("r4");

  s.update_row("r3", make_sfv("3:1"));
  s.remove_row("r2");

  vector<string> ids;
  s.get_all_row_ids(ids);
  sort(ids.begin(), ids.end());

  vector<string> expect;
  expect.push_back("r1");
  expect.push_back("r3");
  expect.push_back("r5");
  expect.push_back("r6");

  EXPECT_EQ(expect, ids);
}

}
}
