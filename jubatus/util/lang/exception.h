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

#ifndef JUBATUS_UTIL_LANG_EXCEPTION_H_
#define JUBATUS_UTIL_LANG_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include "cast.h"

namespace jubatus {
namespace util{
namespace lang{

class parse_error : public std::exception{
public:
  parse_error()
    : filename_("")
    , lineno_(0)
    , pos_(0)
    , msg_(""){
    init();
  }
  parse_error(const std::string &filename,
              int lineno,
              int pos,
              const std::string &msg)
    : filename_(filename)
    , lineno_(lineno)
    , pos_(pos)
    , msg_(msg){
    init();
  }
  ~parse_error() throw() {}

  const char *what() const throw() { return whole_.c_str(); }

  const std::string &filename() const { return filename_; }
  int lineno() const { return lineno_; }
  int pos() const { return pos_; }
  const std::string &msg() const { return msg_; }

private:
  void init(){
    whole_=
      filename_+":"+
      lexical_cast<std::string>(lineno_)+":"+
      lexical_cast<std::string>(pos_)+" "+
      msg_;
  }

  std::string filename_;
  int lineno_;
  int pos_;
  std::string msg_;

  std::string whole_;
};

class end_of_data : public std::runtime_error{
public:
  end_of_data(const std::string& msg)
    : std::runtime_error(msg){}
};

} // lang
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_LANG_EXCEPTION_H_
