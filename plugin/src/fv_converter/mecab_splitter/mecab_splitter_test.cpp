// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/util/concurrent/thread.h"
#include "jubatus/util/lang/bind.h"
#include "jubatus/util/lang/scoped_ptr.h"
#include "jubatus/core/fv_converter/exception.hpp"
#include "mecab_splitter.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::string_feature;
using core::fv_converter::string_feature_element;
using core::fv_converter::converter_exception;

void assert_elements_eq(std::vector<string_feature_element> expected,
                        std::vector<string_feature_element> actual) {
  ASSERT_EQ(expected.size(), actual.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i].begin, actual[i].begin);
    EXPECT_EQ(expected[i].length, actual[i].length);
    EXPECT_EQ(expected[i].value, actual[i].value);
    EXPECT_EQ(expected[i].score, actual[i].score);
  }
}

TEST(mecab_splitter, trivial) {
  mecab_splitter m;
  std::vector<string_feature_element> elems;
  m.extract("本日は晴天なり", elems);
  std::vector<string_feature_element> exp;

  exp.push_back(string_feature_element(0, 6, "本日", 1.0));
  exp.push_back(string_feature_element(6, 3, "は", 1.0));
  exp.push_back(string_feature_element(9, 6, "晴天", 1.0));
  exp.push_back(string_feature_element(15, 6, "なり", 1.0));

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, bigram) {
  mecab_splitter m("", 2, false);
  std::vector<string_feature_element> elems;
  m.extract("本日は晴天なり", elems);
  std::vector<string_feature_element> exp;

  exp.push_back(string_feature_element(0, 9, "本日,は", 1.0));
  exp.push_back(string_feature_element(6, 9, "は,晴天", 1.0));
  exp.push_back(string_feature_element(9, 12, "晴天,なり", 1.0));

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, bigram_with_single_surface) {
  mecab_splitter m("", 2, false);
  std::vector<string_feature_element> elems;
  m.extract("本日", elems);

  ASSERT_EQ(0u, elems.size());
}

TEST(mecab_splitter, base) {
  mecab_splitter m("", 1, true);
  std::vector<string_feature_element> elems;
  m.extract("今日は晴れた", elems);
  std::vector<string_feature_element> exp;

  exp.push_back(string_feature_element(0, 6, "今日", 1.0));    // 今日
  exp.push_back(string_feature_element(6, 3, "は", 1.0));      // は
  exp.push_back(string_feature_element(9, 6, "晴れる", 1.0));  // 晴れ
  exp.push_back(string_feature_element(15, 3, "た", 1.0));     // た

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, base_with_no_base) {
  mecab_splitter m("", 1, true);
  std::vector<string_feature_element> elems;
  m.extract("Jubatusを使った", elems);  // word "Jubatus" has no base form
  std::vector<string_feature_element> exp;

  exp.push_back(string_feature_element(0, 7, "Jubatus", 1.0));  // Jubatus
  exp.push_back(string_feature_element(7, 3, "を", 1.0));       // を
  exp.push_back(string_feature_element(10, 6, "使う", 1.0));    // 使っ
  exp.push_back(string_feature_element(16, 3, "た", 1.0));      // た

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, included_and_not_excluded) {
  mecab_splitter m("", 1, false, "名詞,*|動詞,*", "");

  std::vector<string_feature_element> elems;
  m.extract("寿司を食べる", elems);

  std::vector<string_feature_element> exp;
  exp.push_back(string_feature_element(0, 6, "寿司", 1.0));
  exp.push_back(string_feature_element(9, 9, "食べる", 1.0));

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, included_and_excluded) {
  mecab_splitter m("", 1, false, "*", "助詞,*");

  std::vector<string_feature_element> elems;
  m.extract("噂の真相", elems);

  std::vector<string_feature_element> exp;
  exp.push_back(string_feature_element(0, 3, "噂", 1.0));
  exp.push_back(string_feature_element(6, 6, "真相", 1.0));

  assert_elements_eq(exp, elems);
}

TEST(mecab_splitter, illegal_argument) {
  // Invalid MeCab argument
  EXPECT_THROW(mecab_splitter("-r unknown_file", 1, false),
      converter_exception);

  // Invalid N-gram
  EXPECT_THROW(mecab_splitter("", 0, false), converter_exception);
}

TEST(mecab_splitter_create, trivial) {
  std::map<std::string, std::string> param;
  jubatus::util::lang::scoped_ptr<string_feature> s(create(param));
  std::string d("東京へ行く");
  std::vector<string_feature_element> elems;
  s->extract(d, elems);
  ASSERT_EQ(3u, elems.size());
  ASSERT_EQ(0u, elems[0].begin);
  ASSERT_EQ(6u, elems[0].length);
  ASSERT_EQ(6u, elems[1].begin);
  ASSERT_EQ(3u, elems[1].length);
  ASSERT_EQ(9u, elems[2].begin);
  ASSERT_EQ(6u, elems[2].length);
}

TEST(mecab_splitter_create, illegal_argument) {
  // Invalid MeCab argument
  std::map<std::string, std::string> param1;
  param1["arg"] = "-r unknown_file";
  EXPECT_THROW(
      jubatus::util::lang::scoped_ptr<string_feature>(create(param1)),
      converter_exception);

  // Invalid N-gram
  std::map<std::string, std::string> param2;
  param2["ngram"] = "0";
  EXPECT_THROW(
      jubatus::util::lang::scoped_ptr<string_feature>(create(param2)),
      converter_exception);

  // Invalid base
  std::map<std::string, std::string> param3;
  param3["base"] = "TRUE";  // it should be lower case
  EXPECT_THROW(
      jubatus::util::lang::scoped_ptr<string_feature>(create(param3)),
      converter_exception);
}

TEST(mecab_splitter, with_space) {
  mecab_splitter m;
  std::vector<string_feature_element> elems;
  m.extract(" テスト テスト ", elems);
  std::vector<string_feature_element> exp;

  exp.push_back(string_feature_element(1, 9, "テスト", 1.0));
  exp.push_back(string_feature_element(11, 9, "テスト", 1.0));

  assert_elements_eq(exp, elems);
}

void run(mecab_splitter* m) {
  std::vector<string_feature_element> exp;
  exp.push_back(string_feature_element(0, 6, "本日", 1.0));
  exp.push_back(string_feature_element(6, 3, "は", 1.0));
  exp.push_back(string_feature_element(9, 6, "晴天", 1.0));
  exp.push_back(string_feature_element(15, 6, "なり", 1.0));

  for (int i = 0; i < 1000; ++i) {
    std::vector<string_feature_element> elems;
    m->extract("本日は晴天なり", elems);
    assert_elements_eq(exp, elems);
  }
}

TEST(mecab_spltter, multi_thread) {
  // run mecab_splitter in two threads
  mecab_splitter m;
  std::vector<
      jubatus::util::lang::shared_ptr<jubatus::util::concurrent::thread> > ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(
        jubatus::util::lang::shared_ptr<jubatus::util::concurrent::thread>(
            new jubatus::util::concurrent::thread(
                jubatus::util::lang::bind(&run , &m))));
    ts[i]->start();
  }
  for (int i = 0; i < 100; ++i) {
    ts[i]->join();
  }
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
