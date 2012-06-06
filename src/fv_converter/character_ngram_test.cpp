// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>
#include "character_ngram.hpp"
#include "test_util.hpp"

using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace std;

vector<pair<size_t, size_t> > make_pairs(int*xs) {
  vector<pair<size_t, size_t> > v;
  for (int* x  = xs; *x != -1; x += 2) {
    v.push_back(make_pair(x[0], x[1]));
  }
  return v;
}

TEST(character_ngram, empty) {
  character_ngram ngram(1);
  vector<pair<size_t, size_t> > bs;
  ngram.split("", bs);
  ASSERT_EQ(0u, bs.size());
}

TEST(character_ngram, split) {
  character_ngram ngram(1);

  {
    vector<pair<size_t, size_t> > bs;
    int exp[]= { 0, 1, 1, 1, 2, 1, 3, 1, -1 };
    ngram.split("aa a", bs);
    PairVectorEquals(make_pairs(exp), bs);
  }

  {
    vector<pair<size_t, size_t> > bs;
    int exp[]= { 0, 1, 1, 1, 2, 1, -1 };
    ngram.split("a b", bs);
    PairVectorEquals(make_pairs(exp), bs);
  }
}

TEST(character_ngram, japanese) {
  character_ngram ngram(1);

  vector<pair<size_t, size_t> > bs;
  int exp[]= { 0, 3,  3, 3,  6, 3,  9, 1,  10, 3, -1 };
  ngram.split("あいうaあ", bs);
  PairVectorEquals(make_pairs(exp), bs);
}

TEST(character_ngram, bigram) {
  character_ngram ngram(2);

  vector<pair<size_t, size_t> > bs;
  int exp[]= { 0, 2, 1, 2, 2, 2, -1 };
  ngram.split("aaaa", bs);
  PairVectorEquals(make_pairs(exp), bs);
}
