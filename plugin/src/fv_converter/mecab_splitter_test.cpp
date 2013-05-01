// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/concurrent/thread.h>
#include <pficommon/lang/bind.h>
#include <pficommon/lang/scoped_ptr.h>
#include "jubatus/core/fv_converter/exception.hpp"
#include "mecab_splitter.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::word_splitter;
using core::fv_converter::converter_exception;

TEST(mecab_splitter, trivial) {
  mecab_splitter m;
  std::vector<std::pair<size_t, size_t> > bs;
  m.split("本日は晴天なり", bs);
  std::vector<std::pair<size_t, size_t> > exp;

  exp.push_back(std::make_pair(0, 6));
  exp.push_back(std::make_pair(6, 3));
  exp.push_back(std::make_pair(9, 6));
  exp.push_back(std::make_pair(15, 6));

  ASSERT_EQ(exp, bs);
}

TEST(mecab_splitter, illegal_argument) {
  EXPECT_THROW(mecab_splitter("-r unknown_file"), converter_exception);
}

TEST(mecab_splitter_create, trivial) {
  std::map<std::string, std::string> param;
  pfi::lang::scoped_ptr<word_splitter> s(create(param));
  std::string d("東京へ行く");
  std::vector<std::pair<size_t, size_t> > bs;
  s->split(d, bs);
  ASSERT_EQ(3u, bs.size());
  ASSERT_EQ(0u, bs[0].first);
  ASSERT_EQ(6u, bs[0].second);
  ASSERT_EQ(6u, bs[1].first);
  ASSERT_EQ(3u, bs[1].second);
  ASSERT_EQ(9u, bs[2].first);
  ASSERT_EQ(6u, bs[2].second);
}

TEST(mecab_splitter_create, illegal_argument) {
  std::map<std::string, std::string> param;
  param["arg"] = "-r unknown_file";
  EXPECT_THROW(
      pfi::lang::scoped_ptr<word_splitter>(create(param)),
      converter_exception);
}

TEST(mecab_splitter, with_space) {
  mecab_splitter m;
  std::vector<std::pair<size_t, size_t> > bs;
  m.split(" テスト テスト ", bs);
  std::vector<std::pair<size_t, size_t> > exp;

  exp.push_back(std::make_pair(1, 9));
  exp.push_back(std::make_pair(11, 9));

  ASSERT_EQ(exp, bs);
}

void run(mecab_splitter* m) {
  std::vector<std::pair<size_t, size_t> > exp;
  exp.push_back(std::make_pair(0, 6));
  exp.push_back(std::make_pair(6, 3));
  exp.push_back(std::make_pair(9, 6));
  exp.push_back(std::make_pair(15, 6));

  for (int i = 0; i < 1000; ++i) {
    std::vector<std::pair<size_t, size_t> > bs;
    m->split("本日は晴天なり", bs);
    ASSERT_EQ(exp, bs);
  }
}

TEST(mecab_spltter, multi_thread) {
  // run mecab_splitter in two threads
  mecab_splitter m;
  std::vector<pfi::lang::shared_ptr<pfi::concurrent::thread> > ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(pfi::lang::shared_ptr<pfi::concurrent::thread>(
        new pfi::concurrent::thread(pfi::lang::bind(&run , &m))));
    ts[i]->start();
  }
  for (int i = 0; i < 100; ++i) {
    ts[i]->join();
  }
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
