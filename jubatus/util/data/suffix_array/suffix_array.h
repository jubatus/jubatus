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

#ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_SUFFIX_ARRAY_H_
#define JUBATUS_UTIL_DATA_SUFFIX_ARRAY_SUFFIX_ARRAY_H_

#include "larsson.h"
#include "lcp.h"
#include "checker.h"
#include "invsa.h"

namespace jubatus {
namespace util{
namespace data{
namespace suffix_array{
  namespace sa_options{
    /**
       use_lcp: calc LCP
     */
    enum {
      use_lcp    = (1UL << 0),
      use_inv_sa = (1UL << 1),
      use_child  = (1UL << 2),
      use_bwt    = (1UL << 3),
      do_check   = (1UL << 15),
    };
    enum {
      use_maximal_repeat = use_lcp,
    };
  }

  template<typename T>
  class suffix_array{
    typedef T   element_type;

  public:
    /** 
        Enhanced suffix array wrapper
        
        This constructor creates suffix array from iterator [b, e).

        Symbol '$', which denote the smallest element in the array, is automatically added at the end of array.
        (Thus you do not have to add such things manually.)
        Example: abracadabra -> 10 7 0 3 5 8 1 4 6 9 2
        01234567890

        You can specify some flags, by taking "logical or" of the following:
        sa_options::use_lcp
        sa_options::use_inv_sa
        sa_options::do_check

        @param b iterator that point to the beginning of the array
        @param e iterator that point to the end of the array
        @param record flags that specify extra construction algorithms
    */
    template<typename IT>
    suffix_array(IT b, IT e, 
                 unsigned long record = 0)
      : record(record)
    {
      larsson_suffix_array(b, e, sa_);
      if(record & sa_options::use_lcp)
        lcp(b, e, sa_.begin(), lcp_);
      /*      if(record & use_bwt)
              bwt_ = bwt(b, e, sa_.begin());*/
      if(record & sa_options::use_inv_sa)
        invert_suffix_array(sa_.begin(), sa_.end(), invsa_);
      if(record & sa_options::do_check)
        if(!check_sa(b, e, sa_.begin()))
          assert(false);
    }

    /* destructor */
    ~suffix_array(){
    }

    vector<int> & get_suffix_array(){
      return sa_;
    }

    vector<int> & get_lcp_array(){
      return lcp_;
    }

  private:
    unsigned long record;
    vector<int> sa_;
    vector<int> lcp_;
    vector<T> bwt_;
    vector<int> invsa_;
  };

} // suffix_array
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SUFFIX_ARRAY_SUFFIX_ARRAY_H_
