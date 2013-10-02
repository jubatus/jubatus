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

#ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_LCP_H_
#define JUBATUS_UTIL_DATA_SUFFIX_ARRAY_LCP_H_

#include <vector>
#include <iterator>

namespace jubatus {
namespace util {
namespace data {
namespace suffix_array {
  
  /**
     Calculate Longest Common Prefix (lcp) from container & suffix array
     
     Based on Kasai's algorithm

     @param b container that contains string
     @param e end of iterator
     @param sa suffix array
     @param lcp_ returning array that contains LCP array
  */
  template<typename IT, typename IT2>
  void lcp(IT b, IT e, IT2 sa, std::vector<int> &lcp_)
  {
    const size_t size = distance(b, e);
    std::vector<int> rank(size);
    std::vector<int> height(size);
    for(size_t i = 0; i < size; ++i)
      rank[*(sa+i)] = i;
    size_t h = 0;
    for(size_t i = 0; i < size; ++i){
      if(rank[i] > 0){
        int j = *(sa+rank[i]-1);
        while(*(b+i+h) == *(b+j+h)) ++h;
        height[rank[i]] = h;
        if(h > 0) --h;
      }
    }
    lcp_.swap(height);
  }
} // suffix_array
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_LCP_H_
