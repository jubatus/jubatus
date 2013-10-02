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

#ifndef JUBATUS_UTIL_DATA_STRING_ALGORITHM_H_
#define JUBATUS_UTIL_DATA_STRING_ALGORITHM_H_

#include <vector>
#include <algorithm>
#include <climits>

namespace jubatus {
namespace util {
namespace data {
namespace string {

  template<typename A>
    int edit_distance(const A& a, const A& b){
    std::vector<std::vector<int> > tbl(a.size()+1,std::vector<int>(b.size()+1));
    for (size_t i=0;i<=a.size();++i) tbl[i][0]=i;
    for (size_t i=0;i<=b.size();++i) tbl[0][i]=i;
    for (size_t i=1;i<=a.size();++i)
      for (size_t j=1;j<=b.size();++j) {
        tbl[i][j]=tbl[i-1][j-1]+(a[i-1]!=b[j-1]);
        tbl[i][j]=std::min(tbl[i][j],std::min(tbl[i-1][j],tbl[i][j-1])+1);
      }
    return tbl[a.size()][b.size()];
  }

  template<typename A>
    int longest_common_subsequence(const A& a, const A& b) {
    std::vector<std::vector<int> > tbl(a.size()+1,std::vector<int>(b.size()+1));
    for (size_t i=1;i<=a.size();++i)
      for (size_t j=1;j<=b.size();++j) {
        tbl[i][j]=tbl[i-1][j-1]+(a[i-1]==b[j-1]);
        tbl[i][j]=std::max(tbl[i][j],std::max(tbl[i-1][j],tbl[i][j-1]));
      }
    return tbl[a.size()][b.size()];
  }

  template<typename A>
  int longest_increasing_subsequence(const A& a) {
    if (a.size()==0) return 0;
    
    std::vector<int> tbl(a.size(),INT_MAX),id(a.size());
    for (size_t i=0;i<a.size();++i) {
      id[i]=std::lower_bound(tbl.begin(),tbl.end(),a[i])-tbl.begin();
      tbl[id[i]]=a[i];
    }
    return *max_element(id.begin(),id.end())+1;
  }

} // string
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_STRING_ALGORITHM_H_
