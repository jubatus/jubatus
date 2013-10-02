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

#include "base64.h"

#include <sstream>
#include <stdexcept>
#include <stdint.h>

using namespace std;

namespace pfi{
namespace data{
namespace encoding{

namespace {
const string tochar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int toint(char c)
{
  if (c>='A' && c<='Z') return c-'A';
  if (c>='a' && c<='z') return c-'a'+26;
  if (c>='0' && c<='9') return c-'0'+52;
  if (c=='+') return 62;
  if (c=='/') return 63;
  if (c=='=') return -1;
  throw invalid_argument(string("invalid char: ")+c);
}
}

string base64_encode(const string &s)
{
  size_t index = 0;
  size_t srclen = s.size();
  size_t remain = srclen % 3;
  string result;
  result.reserve(((srclen + 2) / 3) * 4);

  for (size_t i = 0, limit = srclen / 3; i < limit; i++) {
    size_t val = (static_cast<uint8_t>(s[index]) << 16) |
      (static_cast<uint8_t>(s[index + 1]) << 8) | static_cast<uint8_t>(s[index + 2]);
    result.push_back(tochar[val >> 18]);
    result.push_back(tochar[(val>>12) & 0x3f]);
    result.push_back(tochar[(val>>6) & 0x3f]);
    result.push_back(tochar[val & 0x3f]);
    index += 3;
  }

  if (remain) {
    size_t val = (static_cast<uint8_t>(s[index]) << 16) |
      (remain == 2 ? (static_cast<uint8_t>(s[index + 1]) << 8) : 0);
    result.push_back(tochar[val >> 18]);
    result.push_back(tochar[(val>>12) & 0x3f]);

    switch (remain) {
    case 2:
      result.push_back(tochar[(val>>6) & 0x3f]);
      result.push_back('=');
      break;
    case 1:
      result.push_back('=');
      result.push_back('=');
      break;
    default:  // non-reachable
      break;
    }
  }

  return result;
}

string base64_decode(const string &s)
{
  size_t srclen = s.size();
  if (srclen % 4 != 0)
    throw invalid_argument("length cannot be divided by 4");

  size_t blocks = srclen / 4;
  string result;
  result.reserve((srclen / 4) * 3);
  const char* srcptr = s.c_str();

  for (size_t i = 0; i < blocks; i++) {
    int v1 = toint(*srcptr++);
    int v2 = toint(*srcptr++);
    if (v1 == -1 || v2 == -1)
      throw invalid_argument(string("invalid char: ")+s.substr(i * 4,4));
    result.push_back((v1 << 2) | (v2 >> 4));

    int v3 = toint(*srcptr++);
    if (v3 == -1) {
      break;
    }
    result.push_back(((v2 << 4) | (v3 >> 2)) & 0xff);

    int v4 = toint(*srcptr++);
    if (v4 == -1) {
      break;
    }
    result.push_back(((v3 << 6) | v4) & 0xff);
  }

  return result;
}

} // encoding
} // data
} // pfi
