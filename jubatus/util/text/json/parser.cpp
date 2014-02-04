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

#include "parser.h"

#include <iostream>
#include <new>
#include <stack>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "../../lang/exception.h"

namespace jubatus {
namespace util {
namespace text {
namespace json {

class json_builder : public json_parser::callback {
public:
  const json& get() const {
    if (stk.size() != 1)
      throw jubatus::util::lang::parse_error();
    return stk[0];
  }

  void null() {
    stk.push_back(new json_null());
  }

  void boolean(bool val) {
    stk.push_back(new json_bool(val));
  }

  void integer(int64_t val) {
    stk.push_back(new json_integer(val));
  }

  void number(double val) {
    stk.push_back(new json_float(val));
  }

  void string(const char* val, size_t len) {
    stk.push_back(new json_string(std::string(val, val+len)));
  }

  void start_object() {
    ixs.push(stk.size());
  }
  void object_key(const char* val, size_t len) {
    key.push_back(std::string(val, val+len));
  }
  void end_object() {
    json obj(new json_object());
    int ix = ixs.top();
    int jx = stk.size();
    int sz = jx - ix;
    for (int i = 0; i < sz; i++)
      obj.add(key[key.size() - sz + i], stk[ix + i]);
    stk.push_back(obj);
    stk.erase(stk.end() - sz - 1, stk.end() - 1);
    key.erase(key.end() - sz, key.end());
    ixs.pop();
  }
  
  void start_array() {
    ixs.push(stk.size());
  }
  void end_array() {
    json obj(new json_array());
    int ix = ixs.top();
    int jx = stk.size();
    int sz = jx - ix;
    for (int i = 0; i < sz; i++)
      obj.add(stk[ix + i]);
    stk.push_back(obj);
    stk.erase(stk.end() - sz - 1, stk.end() - 1);
    ixs.pop();
  }

  std::vector<json> stk;
  std::vector<std::string> key;

  std::stack<int> ixs;
};

json_parser::json_parser(std::istream& is)
  : it(is), end(), lineno(1), charno(1), cbuf(-1)
{
  buf_len = 256;
  if ((buf = static_cast<char*>(malloc(buf_len))) == 0)
    throw std::bad_alloc();
}

json_parser::~json_parser()
{
  free(buf);
}

json json_parser::parse()
{
  json_builder jb;
  parse_stream(jb);
  return jb.get();
}

void json_parser::parse_stream(callback& cb)
{
  ss();
  if (cbuf < 0 && it == end)
    throw lang::end_of_data("json_parser reached end of data");
  return parse_impl(cb);
}

void json_parser::parse_impl(callback& cb)
{
  ss();
  switch(peek()) {
  case '{':
    parse_object(cb);
    return;

  case '[':
    parse_array(cb);
    return;

  case '0': case '1': case '2': case '3': case '4': 
  case '5': case '6': case '7': case '8': case '9': 
  case '-':
    parse_number(cb);
    return;

  case '\"':
    parse_string(cb);
    return;

  case 'f':
    parse_false(cb);
    return;
  case 'n':
    parse_null(cb);
    return;
  case 't':
    parse_true(cb);
    return;

  default: {
    char err_msg[64];
    snprintf(err_msg, sizeof(err_msg),
             "invalid char: \'%s\' (U+%04X)",
             jubatus::util::data::string::uchar_to_string(peek()).c_str(), peek());
    error(err_msg);
  }

  }
  return; // dmy for supressing warning
}

void json_parser::parse_object(callback& cb)
{
  cb.start_object();

  smatch('{');

  for (int i = 0; ; i++) {
    if (speek() == '}')
      break;
    if (i > 0)
      smatch(',');
    
    ss();
    int len = 0;
    parse_string_prim(buf, buf_len, len);
    cb.object_key(buf, len);
    smatch(':');
    parse_impl(cb);
  }
  
  smatch('}');

  cb.end_object();
}

void json_parser::parse_array(callback& cb)
{
  cb.start_array();

  smatch('[');
  
  for (int i = 0; ; i++) {
    if (speek() == ']')
      break;
    if (i > 0)
      smatch(',');
    
    parse_impl(cb);
  }
  
  smatch(']');

  cb.end_array();
}

void json_parser::parse_number(callback& cb)
{
  std::string src;
  bool is_frac = false;

  if (peek() == '-') {
    src += '-';
    incr();
  }

  while ((cbuf != -1 || it != end) && safe_isdigit(peek())) {
    src += peek();
    incr();
  }

  if (it != end && peek() == '.') {
    is_frac = true;
    src += '.';
    incr();
    while (it != end && safe_isdigit(peek())) {
      src += peek();
      incr();
    }
  }

  if (it != end && (peek() == 'e' || peek() == 'E')) {
    is_frac = true;
    src += peek();
    incr();

    if (it == end)
      error("after exp, digit required.");

    if (peek() == '+' || peek() == '-') {
      src += peek();
      incr();
    }
  }

  while (it != end && safe_isdigit(peek())) {
    src += peek();
    incr();
  }

  const char* srcptr = src.c_str();
  char* endptr;

  if (is_frac) {
    errno = 0;
    double num = strtod(srcptr, &endptr);
    if (errno == ERANGE) {
      if (num == HUGE_VALF || num == HUGE_VALL)
        error("strtod overflow range over. Too huge value for double.");
      else if (num == 0.0)
        error("strtod underflow range over.");
      error("strtod range over.");
    }
    cb.number(num);
  } else {
    errno = 0;
    int64_t num = strtoll(srcptr, &endptr, 10);
    if (errno == ERANGE) {
      if (num == LLONG_MAX || num == LLONG_MIN)
        error("strtoll overflow range over. Too huge value for int64_t.");
      else if (num == 0)
        error("strtoll underflow range over.");
      error("strtoll range over.");
    }
    cb.integer(num);
  }
}

void json_parser::parse_string(callback& cb)
{
  int len = 0;
  parse_string_prim(buf, buf_len, len);
  cb.string(buf, len);
}

void json_parser::parse_string_prim(char*& buf, int& buf_len, int& str_len)
{
  char* p = buf;

  match('\"');
  
  for (;;) {
    if (p+8 >= buf+buf_len) {
      size_t adv = p - buf;
      if (char* newbuf = static_cast<char*>(realloc(buf, 2*buf_len)))
        buf = newbuf;
      else
        throw std::bad_alloc();
      buf_len *= 2;
      p = buf + adv;
    }

    if (peek() == '\"')
      break;
    
    if (peek() == '\\') {
      incr();

      int c = incr();
      
      switch (c) {
      case '\"': *p++ = '\"'; break;
      case '\\': *p++ = '\\'; break;
      case '/':  *p++ = '/';  break;
      case 'b':  *p++ = '\b'; break;
      case 'f':  *p++ = '\f'; break;
      case 'n':  *p++ = '\n'; break;
      case 'r':  *p++ = '\r'; break;
      case 't':  *p++ = '\t'; break;
        
      case 'u': {
        int a = parse_hex();
        int b = parse_hex();
        int c = parse_hex();
        int d = parse_hex();
        jubatus::util::data::string::uchar_to_chars((a<<12)|(b<<8)|(c<<4)|d, p);
        break;
      }

      default: {
        char err_msg[64];
        snprintf(err_msg, sizeof(err_msg),
                 "unexpected unescaped char: \'%s\' (U+%04X)",
                 jubatus::util::data::string::uchar_to_string(c).c_str(), c);
        error(err_msg);
      }

      }
    }
    else {
      int c = incr();
      
      if ((c>=0x20 && c<=0x21) ||
          (c>=0x23 && c<=0x5B) ||
          (c>=0x5D && c<=0x10FFFF))
        jubatus::util::data::string::uchar_to_chars(c, p);
      else {
        char err_msg[64];
        snprintf(err_msg, sizeof(err_msg),
                 "unexpected unescaped char: \'%s\' (U+%04X)",
                 jubatus::util::data::string::uchar_to_string(c).c_str(), c);
        error(err_msg);
      }
    }
  }
  
  match('\"');

  str_len = p - buf;
}

void json_parser::parse_false(callback& cb)
{
  match('f');
  match('a');
  match('l');
  match('s');
  match('e');
  cb.boolean(false);
}

void json_parser::parse_null(callback& cb)
{
  match('n');
  match('u');
  match('l');
  match('l');
  cb.null();
}

void json_parser::parse_true(callback& cb)
{
  match('t');
  match('r');
  match('u');
  match('e');
  cb.boolean(true);
}

void json_parser::error(const std::string& msg)
{
  std::string filename="<istream>";
  
  throw jubatus::util::lang::parse_error(filename, lineno, charno, msg);
}

} // json
} // text
} // util
} // jubatus
