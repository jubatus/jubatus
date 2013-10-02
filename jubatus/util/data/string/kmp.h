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

#ifndef INCLUDE_GUARD_PFI_DATA_STRING_KMP_H_
#define INCLUDE_GUARD_PFI_DATA_STRING_KMP_H_

#include <iterator>
#include <vector>

namespace jubatus {
namespace util{
namespace data{
namespace string{

template<class T>
class kmp {
public:
 explicit kmp(const T& q) {
   query=q;
   next=std::vector<int>(query.size());
   int t=next[0]=0;
   for (int i=1;i<(int)query.size();++i) {
     while(t>0&&query[i]!=query[t]) t=next[t-1];
     if (query[i]==query[t]) next[i]=++t;
     else next[i]=0;
   }
 }

 size_t find(const T& text, int offset = 0) const
 {
   int t=0;
   for (int i=offset;i<(int)text.size();++i) {
     while(t>0&&text[i]!=query[t]) t=next[t-1];
     if (text[i]==query[t]) ++t;
     if (t==(int)query.size()) return i+1-query.size();
   }
   return std::string::npos;
 }

private:
 T query;
 std::vector<int> next;
};

} // string
} // data
} // util
} // jubatus
#endif // #ifndef INCLUDE_GUARD_PFI_DATA_STRING_KMP_H_
