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

#include "config_file.h"

#include <cctype>
#include <cstring>
#include <fstream>
#include <string>

#include "../lang/exception.h"

using namespace std;
using namespace jubatus::util::lang;

namespace jubatus {
namespace util {
namespace data {

config_file::config_file(const string& filename)
{
  ifstream ifs(filename.c_str());
  init(filename, ifs);
}

config_file::config_file(istream& is)
{
  init("", is);
}

static string trim(const string& s)
{
  int st = 0, ed = s.length()-1;
  while (st < (int)s.length() && isspace(s[st]))
    ++st;
  while (ed >= 0 && isspace(s[ed]))
    --ed;
  if (st > ed)
    return "";
  return s.substr(st, ed-st+1);
}

static const char* skip_spaces(const char* p)
{
  while (isspace(*p))
    ++p;
  return p;
}

void config_file::init(const string& filename, istream& is)
{
  string cur_sect;

  int lineno = 1;
  for (string line; getline(is, line); ++lineno) {
    const char* p = line.c_str();
    const char* st = p;
    p = skip_spaces(p);

    if (*p=='\0') continue;
    if (*p==';') continue;

    if (*p=='[') {
      const char* q = strchr(p, ']');
      if (q == NULL)
        throw parse_error(filename, lineno, line.length(), "']' missing");
      cur_sect = string(p+1, q);
      q = skip_spaces(q+1);
      if (*q != '\0') 
        throw parse_error(filename, lineno, q-st, "invalid character");

      (void)dat[cur_sect];

      continue;
    }

    const char* q = strchr(p, '=');
    if (q == NULL)
      throw parse_error(filename, lineno, line.length(), "'=' missing");

    string key(trim(string(p, q)));
    string val(trim(q+1));

    if (val.length() >= 2) {
      if (val[0] == '"' &&
          val[val.length()-1] == '"')
        val.assign(val, 1, val.length()-2);
    }

    dat[cur_sect][key] = val;
  }
}

} // data
} // util
} // jubatus
