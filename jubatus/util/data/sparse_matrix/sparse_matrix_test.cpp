// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <gtest/gtest.h>

#include "./sparse_matrix.h"

#include <climits>
#include <cstddef>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace jubatus::util::data::sparse_matrix;

static const string tmp_file="./tmp";

void clean() {
  unlink(tmp_file.c_str());
  unlink((tmp_file+".offset").c_str());
}

// create empty index
TEST(sparse_matrix, index_with_no_item) {
  clean();

  {
    sparse_matrix_writer smw;
    EXPECT_EQ(smw.open(tmp_file),0);
    smw.close();
  }
  {  
    sparse_matrix_reader smr;
    EXPECT_EQ(smr.open(tmp_file),0);
    smr.close();
  }

  clean();
}

TEST(sparse_matrix_test, index) {
  clean();

  srandom(time(NULL));

  vector<vector<pair<int,unsigned char> > > mat;
  for (int i=0;i<100;++i) {
    int num=random()%100+1;
    vector<pair<int,unsigned char> > row;
    int c=-1;
    for (int j=0;j<num;++j) {
      c+=random()%255+1;
      row.push_back(make_pair(c,random()%255+1));
    }
    mat.push_back(row);
  }

  {
    sparse_matrix_writer smw;
    smw.open(tmp_file);
    for (int i=0;i<(int)mat.size();++i) smw.append_row(mat[i]);
    smw.close();
  }

  {
    sparse_matrix_reader smr;
    smr.open(tmp_file);
    EXPECT_EQ(mat.size(),size_t(smr.row_num()));
    for (int i=0;i<(int)mat.size();++i) {
      { // get_row using vector<pair<int,unsigned char> >
        vector<pair<int,unsigned char> > row;
        smr.get_row(i,row);
        EXPECT_EQ(mat[i].size(),row.size());
        for (int j=0;j<(int)mat[i].size();++j) {
          EXPECT_EQ(row[j].first,mat[i][j].first);
          EXPECT_EQ((int)row[j].second,(int)mat[i][j].second);
        }
      }
      { // get_row using map<int,unsigned char>
        map<int,unsigned char> row;
        smr.get_row(i,row);
        EXPECT_EQ(mat[i].size(),row.size());
        map<int,unsigned char>::iterator it=row.begin();
        for (int j=0;j<(int)mat[i].size()&&it!=row.end();++j,++it) {
          EXPECT_EQ(it->first,mat[i][j].first);
          EXPECT_EQ((int)it->second,(int)mat[i][j].second);
        }
      }
      { // get_row using unordered_map<int,unsigned char>
        jubatus::util::data::unordered_map<int,unsigned char> row_um;
        vector<pair<int,unsigned char> > row;
        smr.get_row(i,row_um);
        for (jubatus::util::data::unordered_map<int,unsigned char>::iterator it=row_um.begin();it!=row_um.end();++it)
          row.push_back(make_pair(it->first,it->second));
        sort(row.begin(),row.end());
        
        EXPECT_EQ(mat[i].size(),row.size());
        for (int j=0;j<(int)mat[i].size();++j) {
          EXPECT_EQ(row[j].first,mat[i][j].first);
          EXPECT_EQ((int)row[j].second,(int)mat[i][j].second);
        }
      }
    }
  }

  clean();
}

