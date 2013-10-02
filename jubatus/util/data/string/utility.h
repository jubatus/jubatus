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

#ifndef JUBATUS_UTIL_DATA_STRING_UTILITY_H_
#define JUBATUS_UTIL_DATA_STRING_UTILITY_H_

#include <vector>
#include <algorithm>

#include <ctype.h>

namespace jubatus {
namespace util{
namespace data{
namespace string{

template <class String, class Char, class Iterator>
void split(const String &str, Char sep, Iterator p)
{
  typename String::const_iterator bef = str.begin(), end = str.end();
  typename String::const_iterator cur = bef;
  for (;;) {
    while(cur != end && *cur != sep) ++cur;
    *p = String(bef, cur);
    ++p;
    if (cur == end) break;
    bef = ++cur;
  }
}

template <class Container, class String, class Char>
inline Container split(const String &str, Char sep)
{
  Container ret;
  split(str, sep, std::back_inserter(ret));
  return ret;
}

template <class String, class Char>
inline std::vector<String> split(const String &str, Char sep)
{
  return split<std::vector<String> >(str, sep);
}

template <class String, class Iterator>
void split(const String &str, const String &sep, Iterator p)
{
  if (sep.begin() == sep.end()) return;

  typename String::const_iterator bef = str.begin(), end = str.end();
  for (;;) {
    typename String::const_iterator cur =
      std::search(bef, end, sep.begin(), sep.end());
    *p = String(bef, cur);
    ++p;
    if (cur == end) break;
    std::advance(cur, std::distance(sep.begin(), sep.end()));
    bef = cur;
  }
}

template <class Container, class String>
inline Container split(const String &str, const String &sep)
{
  Container ret;
  split(str, sep, std::back_inserter(ret));
  return ret;
}

template <class String>
inline std::vector<String> split(const String &str, const String &sep)
{
  return split<std::vector<String> >(str, sep);
}

template <class String>
void strip(const String &str, String &res)
{
  typename String::const_iterator beg = str.begin();
  typename String::const_iterator end = str.end();

  while (beg != end && isspace(*beg)) ++beg;

  while (beg != end) {
    --end;
    if (!isspace(*end)) {
      end++;
      break;
    }
  }

  String(beg, end).swap(res);
}

template <class String>
inline String strip(const String &str)
{
  String ret;
  strip(str, ret);
  return ret;
}

template <class String>
void lstrip(const String &str, String &res)
{
  typename String::const_iterator beg = str.begin();
  typename String::const_iterator end = str.end();

  while (beg != end && isspace(*beg)) ++beg;

  String(beg, end).swap(res);
}

template <class String>
inline String lstrip(const String &str)
{
  String ret;
  lstrip(str, ret);
  return ret;
}

template <class String>
void rstrip(const String &str, String &res)
{
  typename String::const_iterator beg = str.begin();
  typename String::const_iterator end = str.end();

  while (beg != end) {
    --end;
    if (!isspace(*end)) {
      end++;
      break;
    }
  }

  String(beg, end).swap(res);
}

template <class String>
inline String rstrip(const String &str)
{
  String ret;
  rstrip(str, ret);
  return ret;
}

template <class Iterator, class String>
void join(Iterator begin, Iterator end, const String &sep, String &res)
{
  String ret;
  bool first = true;

  for (Iterator p = begin; p != end; p++) {
    if (first) first = false;
    else ret += sep;
    ret += *p;
  }

  ret.swap(res);
}

template <class Container, class String>
inline void join(const Container &strs, const String &sep, String &res)
{
  join(strs.begin(), strs.end(), sep, res);
}

template <class Iterator, class String>
inline String join(Iterator begin, Iterator end, const String &sep)
{
  String ret;
  join(begin, end, sep, ret);
  return ret;
}

template <class Container, class String>
inline String join(const Container &strs, const String &sep)
{
  return join(strs.begin(), strs.end(), sep);
}

template <class String>
bool starts_with(const String &str, const String &prefix)
{
  if (str.size() < prefix.size()) return false;
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

template <class String>
bool ends_with(const String &str, const String &suffix)
{
  if (str.size() < suffix.size()) return false;
  return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

template <class String>
void replace(const String &str, const String &from, const String &to, String &res)
{
  if (from.begin() == from.end()) {
    res = str;
    return;
  }

  String ret;
  typename String::const_iterator bef = str.begin(), end = str.end();
  for (;;) {
    typename String::const_iterator cur =
      std::search(bef, end, from.begin(), from.end());
    ret.append(bef, cur);
    if (cur == end) break;
    ret += to;
    std::advance(cur, std::distance(from.begin(), from.end()));
    bef = cur;
  }

  ret.swap(res);
}

template <class String>
inline String replace(const String &str, const String &from, const String &to)
{
  String ret;
  replace(str, from, to, ret);
  return ret;
}

} // string
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_STRING_UTILITY_H_
