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

#ifndef JUBATUS_UTIL_DATA_SERIALIZATION_IOSTREAM_H_
#define JUBATUS_UTIL_DATA_SERIALIZATION_IOSTREAM_H_

#include "base.h"

#include <iostream>
#include <fstream>
#include <cstdio>

#include <unistd.h>

#include "../../lang/shared_ptr.h"
#include "../../system/file.h"

namespace jubatus {
namespace util{
namespace data{
namespace serialization{

namespace path{
class tmp_path{
public:
  const char *operator()() const{
    return "/tmp";
  }
};
}

template <class PathFunc=path::tmp_path>
class stream{
public:
  stream()
    : fname(PathFunc()()+std::string("/serializeXXXXXX"))
    , spios(jubatus::util::system::file::tmpstream(fname))
    , pios(spios.get()){
  }

  stream(PathFunc pf)
    : fname(pf()+std::string("/serializeXXXXXX"))
    , spios(jubatus::util::system::file::tmpstream(fname))
    , pios(spios.get()){
  }

  stream(std::iostream &ios)
    :pios(&ios){}

  ~stream(){
    pios=NULL;
    spios.reset();
    if (fname!="")
      unlink(fname.c_str());
  }

  const std::string &name() const { return fname; }
  std::iostream &get() const { return *pios; }

private:
  std::string fname;
  jubatus::util::lang::shared_ptr<std::iostream> spios;
  std::iostream *pios;
};

template <class Archive>
void serialize(Archive &ar, std::iostream &ios)
{
  // encode:
  // 00-fe -> sonomama
  // ff    -> ff ff
  // EOF   -> ff 00

  if (ar.is_read){
    std::ostreambuf_iterator<char> p(ios);
    char c[2]={};
    for (;;){
      ar & c[0];
      if (static_cast<unsigned char>(c[0])==0xff){
        ar & c[1];
        if (c[1]==0x00)
          break;
        *p=0xff;
      }
      else
        *p=c[0];
    }
    ios.seekg(0);
  }
  else{
    std::istreambuf_iterator<char> p(ios), end;
    char c[]={'\0', '\xff'};
    while(p!=end){
      c[0]=*p++;
      if (static_cast<unsigned char>(c[0])==0xff)
        ar & c[0] & c[1];
      else
        ar & c[0];
    }
    c[0]='\xff';
    c[1]='\0';
    ar & c[0] & c[1];
  }
}

template <class Archive, class Func>
void serialize(Archive &ar, stream<Func> &str)
{
  serialize(ar, str.get());
}

template <class Archive, class Func>
void serialize(Archive &ar, const stream<Func> &str)
{
  serialize(ar, str.get());
}

class stream_type : public type_rep{
public:
  stream_type(){}

  void traverse(jubatus::util::lang::function<void(type_rep*)> f){
    f(this);
  }

  void print(std::ostream &os){
    os<<"stream";
  }
};

template <class Func>
void serialize(reflection &ref, stream<Func> &str)
{
  ref.add("", jubatus::util::lang::shared_ptr<type_rep>(new stream_type()));
}

} // serializatin
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SERIALIZATION_IOSTREAM_H_
