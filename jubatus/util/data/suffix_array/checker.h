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

#ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_CHECKER_H_
#define JUBATUS_UTIL_DATA_SUFFIX_ARRAY_CHECKER_H_

#include <map>
#include <iterator>

namespace jubatus {
namespace util{
namespace data {
namespace suffix_array {
  /**
     Check whether suffix array is correctly constructed
     
     @param sb Original string iterator
     @param se End of original string iterator 
     @param sa begining iterator of suffix array container
     @return This function always returns true (otherwise assert(false) is executed)
  */
  template<typename IT, typename IT2>
  bool check_sa(IT sb, IT se, IT2 sa)
  {
    size_t n = distance(sb, se);
    {
      // sa[i] <- [0, n)
      for(size_t i = 0; i < n; ++i){
        if(*(sa+i) >= n || *(sa+i) < 0)
          return false;
      }
    }
    {
      for(size_t i = 1; i < n; ++i){
        if(*(sb+*(sa+i-1)) > *(sb+*(sa+i)))
          return false;
      }
    }
    {
      typedef __typeof__(*sb) T;
      std::map<T, size_t> a;
      for(size_t i = n - 1; /* */ ; --i){
        a[*(sb+*(sa+i))] = i;
        if(i == 0) break; // i >= 0 -> inf loop
      }
      ++a[*(sb+(n-1))];
      for(int k = 0; k < int(n); ++k)
        if(*(sa+k) > 0){
          T c = *(sb+(*(sa+k)-1));
          if(*(sa+a[c])+1 != *(sa+k)){
            return false;
          }
          ++a[c];
        }
    }
    return true;
  }
} // suffix_array
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_CHECKER_H_
