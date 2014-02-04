// Copyright (c)2008-2012, Preferred Infrastructure Inc.
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

#ifndef JUBATUS_UTIL_TEXT_JSON_PARSER_H_
#define JUBATUS_UTIL_TEXT_JSON_PARSER_H_

#include <iterator>

#include "../../lang/exception.h"
#include "../../data/string/ustring.h"

#include "base.h"

namespace jubatus {
namespace util {
namespace text {
namespace json {

class json_parser {
public:
  json_parser(std::istream& is);
  ~json_parser();

  json parse();

  class callback {
  public:
    virtual ~callback() {}

    virtual void null() {}
    virtual void boolean(bool) {}
    virtual void integer(int64_t) {}
    virtual void number(double) {}
    virtual void string(const char*, size_t /* length */) {}

    virtual void start_object() {}
    virtual void object_key(const char*, size_t /* length */) {}
    virtual void end_object() {}

    virtual void start_array() {}
    virtual void end_array() {}
  };

  void parse_stream(callback& cb);

private:
  void parse_impl(callback& cb);
  void parse_object(callback& cb);
  void parse_array(callback& cb);
  void parse_number(callback& cb);
  void parse_string(callback& cb);
  void parse_false(callback& cb);
  void parse_null(callback& cb);
  void parse_true(callback& cb);

  void parse_string_prim(char*& buf, int& buf_len, int& str_len);

  int parse_hex() {
    int c = incr();
    if (safe_isdigit(c))
      return c - '0';
    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;
    
    error(std::string("hex required but got \'")+(char)c+"\'");
    return 0;
  }

  int peek() {
    if (cbuf < 0) {
      if (it != end) {
        cbuf = jubatus::util::data::string::chars_to_uchar(it, end);
      } else {
        throw jubatus::util::lang::end_of_data("json_parser reached end of data");
      }
    }
    return cbuf;
  }
  int incr() {
    int c = peek();
    cbuf = -1;
    if (c == '\n') {
      lineno++;
      charno=1;
    } else
      charno++;
    return c;
  }
  void ss() {
    while(it != end && safe_isspace(peek()))
      incr();
  }

  int safe_isspace(int c) {
    if (c >= 0x100)
      return 0;
    if (c < 0)
      return 0;
    return isspace(c);
  }
  int safe_isdigit(int c) {
    if (c >= 0x100)
      return 0;
    if (c < 0)
      return 0;
    return isdigit(c);
  }

  void match(char c) {
    if (peek() != c)
      error(std::string("\'")+c+"\' is required but got \'"+(char)peek()+"\'");
    incr();
  }

  int speek() {
    ss();
    return peek();
  }

  void smatch(char c) {
    ss();
    match(c);
  }

  void error(const std::string& msg);

  std::istreambuf_iterator<char> it, end;

  int lineno, charno;
  int cbuf;

  char* buf;
  int buf_len;
};

inline std::istream& operator>>(std::istream& is, json& j)
{
  j=json_parser(is).parse();
  return is;
}

} // json
} // text
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_TEXT_JSON_PARSER_H_
