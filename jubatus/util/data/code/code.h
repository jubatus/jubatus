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

#ifndef JUBATUS_UTIL_DATA_CODE_CODE_H_
#define JUBATUS_UTIL_DATA_CODE_CODE_H_

#include <cstdio>
#include <vector>
#include <ostream>
#include <istream>

namespace jubatus {
namespace util {
namespace data {
namespace code {
  class encoder {
  public:
    encoder();
    /**
     * @brief write encoded bitstream
     * @return 0 for success, -1 for error
     */
    int flush(std::string fn);
    int flush(std::ostream& os);

    std::vector<unsigned char>& get_bytes();

    /**
     * @brief write raw bytes
     * @param v byte to write
     */
    void byte(unsigned char v);
    /**
     * @brief write byte using EXACT len bits
     * @param v byte to write
     * @param len byte num
     */
    void word_with_length(unsigned int v, unsigned int len);
    
    /**
     * @brief write gamma code
     * @param v int to write
     */    
    void gamma(unsigned int v);
    /**
     * @brief write delta code
     * @param v int to write
     */
    void delta(unsigned int v);

    /**
     * @brief write rice code
     * @param v int to write
     * @param k parameter
     * rice coding:
     * write upper bits above k bit by gamma code,
     * lower k bits directory.
     */
    void rice(unsigned int v, unsigned int k);
    
    /**
     * @brief write prefix code
     * @param v write to int
     */
    void prefix_code(unsigned int v);

  private:
    unsigned int bit;
    std::vector<unsigned char> bytes;
  };

  class decoder {
  public:
    decoder();
    ~decoder();

    bool is_open();
    int attach(std::string fn);
    int attach(std::istream& is, int size);
    int attach(unsigned char* buf);
    void detach();
    void seek(int pos, int bit=0);

    unsigned char byte();
    unsigned int word_with_length(unsigned int len);
    unsigned int gamma();
    unsigned int delta();
    unsigned int rice(unsigned int k);
    unsigned int prefix_code();
  private:
    bool newed;
    unsigned int pos;
    unsigned int bit;
    unsigned char* bytes;
  };

} // code
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_CODE_CODE_H_
