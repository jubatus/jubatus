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

#include "code.h"

#include <algorithm>
#include <fstream>
#include <cassert>

#include "../../system/file.h"
#include "../../system/endian_util.h"

using namespace std;
using namespace jubatus::util::system::file;
using namespace jubatus::util::system::endian;


namespace jubatus {
namespace util {
namespace data {
namespace code {

  inline unsigned int get_len(unsigned int v) {
    return 32-__builtin_clz(v);
  }

////////////////////////////////////////////////////////////////
//
// encoder
//

  encoder::encoder():bit(0) { }

  int encoder::flush(string fn) {
    ofstream ofs(fn.c_str());
    return flush(ofs);
  }

  int encoder::flush(ostream& os) {
    int res=bytes.size();
    os.write((char*)&bytes[0],bytes.size());
    bytes.clear();
    bit=0;
    return res;
  }

  vector<unsigned char>& encoder::get_bytes() {
    return bytes;
  }

  void encoder::byte(unsigned char v) {
    bit=0;
    bytes.push_back(v);
  }

  void encoder::word_with_length(unsigned int v, unsigned int len)
  {
    unsigned int rest=8-bit;
    while(len) {
      if (bit==0) bytes.push_back(0);
      unsigned int consume=min(rest,len);
      unsigned int vv=v&((1U<<consume)-1);
      bytes.back()|=(vv<<bit);
      v>>=consume;
      len-=consume;
      bit=(bit+consume)&7;
      rest=8;
    }
  }

  void encoder::gamma(unsigned int v)
  {
    unsigned int pmsf=get_len(v)-1;     // the position of the most significant bit
    word_with_length(0,pmsf);           // 0
    word_with_length(1,1);              // 1
    word_with_length(v-(1U<<pmsf),pmsf); // value
  }

  void encoder::delta(unsigned int v)
  {
    unsigned int pmsf=get_len(v)-1;       // the position of the most significant bit
    gamma(pmsf+1);
    word_with_length(v-(1<<pmsf),pmsf);
  }

  void encoder::rice(unsigned int v, unsigned int k)
  {
    delta((v>>k)+1);
    word_with_length(v&((1<<k)-1),k);
  }

  void encoder::prefix_code(unsigned int v)
  {
    bit=0;
    while(v>=128) {
      bytes.push_back(v&127);
      v>>=7;
    } 
    bytes.push_back(v+128);
  }

////////////////////////////////////////////////////////////////
//
// decoder
//

  decoder::decoder():newed(false),pos(0),bit(0),bytes(NULL) 
  { 
  }

  decoder::~decoder()
  {
    detach();
  }

  bool decoder::is_open() {
    return bytes!=0;
  }

  int decoder::attach(string fn) {
    ifstream ifs(fn.c_str());
    if (!ifs) return -1;
    return attach(ifs,get_file_size(fn));
  }

  int decoder::attach(istream& is, int size) {
    detach();
    bytes=new unsigned char[size+sizeof(long long)];
    newed=true;
    is.read((char*)&bytes[0],size);
    for (int i=size;i<(int)(size+sizeof(long long));++i) bytes[i]=0;
    return 0;
  }

  int decoder::attach(unsigned char* buf)
  {
    detach();
    newed=false;
    bytes=buf;
    return 0;
  }

  void decoder::detach()
  {
    if (bytes&&newed) delete[] bytes;
    newed=false;
    bytes=NULL;
    pos=bit=0;
  }

  void decoder::seek(int pos_, int bit_) {
    pos=pos_;
    bit=bit_;
  }

  unsigned char decoder::byte() {
    return bytes[pos++];
  }

  unsigned int decoder::word_with_length(unsigned int len)
  {
    unsigned int v=to_little((*(long long*)&bytes[pos]))>>bit;
    bit+=len;
    pos+=bit>>3;
    bit&=7;
    return v&((1ULL<<len)-1);  
  }

  unsigned int decoder::gamma()
  {
    long long v=to_little((*(long long*)&bytes[pos]))>>bit;
    unsigned int pmsf=__builtin_ctzll(v); 
    bit+=pmsf+1;
    pos+=bit>>3;
    bit&=7;

    unsigned int res=word_with_length(pmsf);
    return (1U<<pmsf)+res;
  }

  unsigned int decoder::delta()
  {
    int len=gamma()-1;
    return (1<<len)+word_with_length(len);
  }

  unsigned int decoder::rice(unsigned int k)
  {
    unsigned int res=(delta()-1)<<k;
    res+=word_with_length(k);
    return res;
  }

  unsigned int decoder::prefix_code()
  {
    if (bit) {
      ++pos;
      bit=0;
    }

    unsigned int res=0;
    unsigned int k=1;
    while(!(bytes[pos]&128)) {
      res+=bytes[pos]*k;
      ++pos;
      k<<=7;
    }
    res+=(bytes[pos]-128)*k;
    ++pos;

    return res;
  }
  

} // code
} // data
} // util
} // jubatus
