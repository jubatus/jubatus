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

#ifndef JUBATUS_UTIL_DATA_DIGEST_MD5_H_
#define JUBATUS_UTIL_DATA_DIGEST_MD5_H_

#include <iostream>
#include <string>
#include <stdint.h>

namespace jubatus {
namespace util{
namespace data{
namespace digest{

class itr_buf_base{
public:
  itr_buf_base(){}
  virtual ~itr_buf_base(){}
  virtual bool empty() const = 0;
  virtual void get(uint32_t*) = 0;
};

template <class Iterator>
class itr_buf : public itr_buf_base{
public:
  itr_buf(Iterator p, Iterator q)
    : p(p), q(q), ovr(0x80), end(false), cnt(0) {}

  bool empty() const{
    return end;
  }

  void get(uint32_t *x){
    x[0]=get();
    x[1]=get();
    x[2]=get();
    x[3]=get();
    x[4]=get();
    x[5]=get();
    x[6]=get();
    x[7]=get();
    x[8]=get();
    x[9]=get();
    x[10]=get();
    x[11]=get();
    x[12]=get();
    x[13]=get();

    if (p==q && ovr==0){
      end=true;
      x[14]=static_cast<uint32_t>(cnt<<3);
      x[15]=static_cast<uint32_t>(cnt>>29);
    }
    else{
      x[14]=get();
      x[15]=get();
    }
  }

private:
  uint32_t get(){
    uint32_t a=get_byte();
    uint32_t b=get_byte();
    uint32_t c=get_byte();
    uint32_t d=get_byte();
    return a|(b<<8)|(c<<16)|(d<<24);
  }

  uint8_t get_byte(){
    if (p!=q){
      char c=*p++;
      cnt++;
      return static_cast<uint8_t>(c);
    }
    else{
      uint8_t ret=ovr;
      ovr=0;
      return ret;
    }
  }

  Iterator p, q;
  uint8_t ovr;
  bool end;
  size_t cnt;
};

class md5sum{
public:
  md5sum():a(0), b(0), c(0), d(0) {}
  md5sum(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
    :a(a), b(b), c(c), d(d) {}

  template <class Iterator>
  md5sum(Iterator p, Iterator q){
    itr_buf<Iterator> buf(p, q);
    calc(buf);
  }

  explicit md5sum(std::istream &is){
    std::istreambuf_iterator<char> p(is), q;
    itr_buf<std::istreambuf_iterator<char> > buf(p, q);
    calc(buf);
  }

  explicit md5sum(const std::string &s){
    itr_buf<std::string::const_iterator> buf(s.begin(), s.end());
    calc(buf);
  }

  uint32_t a,b,c,d;

private:  
  void calc(itr_buf_base &buf);
};

bool operator<(const md5sum &a, const md5sum &b);

std::ostream &operator<<(std::ostream &os, const md5sum &m);

} // digest
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_DIGEST_MD5_H_
