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

#ifndef JUBATUS_UTIL_DATA_STRING_USTRING_H_
#define JUBATUS_UTIL_DATA_STRING_USTRING_H_

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <stdexcept>

namespace jubatus {
namespace util {
namespace data {
namespace string {

typedef uint32_t uchar;

class ustring : public std::basic_string<uchar> {
public:
  ustring(){}
  ustring(const ustring &s, size_type pos = 0, size_type n = npos)
    : std::basic_string<uchar>(s, pos, n) {}
  ustring(const uchar *p)
    : std::basic_string<uchar>(p) {}
  ustring(const uchar *p, size_type n)
    : std::basic_string<uchar>(p, n) {}
  ustring(size_type n, uchar c)
    : std::basic_string<uchar>(n, c) {}

  template <class InputIterator>
  ustring(InputIterator first, InputIterator last)
    : std::basic_string<uchar>(first, last) {}

  ustring(const std::basic_string<uchar> &str)
    : std::basic_string<uchar>(str) {}
};

// string <-> ustring conversion
ustring string_to_ustring(const char* p);
ustring string_to_ustring(const std::string& s);
std::string ustring_to_string(const ustring& us);

// get first character of string
uchar string_to_uchar(const char* p);
uchar string_to_uchar(const std::string& s);
std::string uchar_to_string(uchar uc);

namespace detail {
template <class InputIterator1, class InputIterator2>
uchar chars_to_uchar_impl(InputIterator1& in, InputIterator2 end)
{
  if (in == end)
    throw std::invalid_argument("Empty string");

  if (((*in) & 0x80) == 0) // U+0000 to U+007F
    return *in++;

  const unsigned c = *in & 0xFF;
  if (c < 0xC2)
    throw std::invalid_argument("Invalid UTF-8");
  if (c > 0xFD)
    throw std::invalid_argument("Invalid UTF-8");

  static const uchar head_masks[] = { 0xE0, 0xF0, 0xF8 };
  static const uchar tail_masks[] = { 0x1F, 0x0F, 0x07 };
  static const uchar flag_bits[] = { 0xC0, 0xE0, 0xF0 };

  uchar ret;
  int nbytes = 0;

  for (size_t i = 0; i < sizeof(head_masks)/sizeof(head_masks[0]); ++i)
    if ((*in & head_masks[i]) == flag_bits[i]) {
      ret = *in++ & tail_masks[i];
      nbytes = i + 2;
      break;
    }

  if (nbytes == 0)
    throw std::invalid_argument("Invalid UTF-8");

  for (int i = 1; i < nbytes; ++i) {
    if (in == end)
      throw std::invalid_argument("Too short string");
    if ((*in & 0xC0) != 0x80)
      throw std::invalid_argument("Invalid UTF-8");
    ret <<= 6;
    ret |= *in++ & 0x3F;
  }

  static const uchar mins[] = { 0, 0, 0x80, 0x800, 0x10000 };
  static const uchar maxs[] = { 0, 0, 0x7FF, 0xFFFF, 0x1FFFFF };

  if (ret < mins[nbytes])
    throw std::invalid_argument("Invalid UTF-8");
  if (ret > maxs[nbytes])
    throw std::invalid_argument("Invalid UTF-8");

  return ret;
}

struct dummy_end_iterator {};

template <class Iterator>
bool operator==(Iterator, dummy_end_iterator)
{
    return false;
}
}

#ifdef __GNUG__
template <class InputIterator>
uchar chars_to_uchar(InputIterator&) __attribute__((deprecated));
#endif

// char[] -> uchar conversion
template <class InputIterator>
uchar chars_to_uchar(InputIterator& in)
{
  return detail::chars_to_uchar_impl(in, detail::dummy_end_iterator());
}

template <class InputIterator>
uchar chars_to_uchar(InputIterator& in, InputIterator end)
{
  return detail::chars_to_uchar_impl(in, end);
}

// uchar -> char[] conversion
template <class OutputIterator>
void uchar_to_chars(uchar c, OutputIterator &out){
  char b;
  if(c <= 0x007f) {
    b = c;
    *out++ = b;
  } else if (0x0080 <= c && c <= 0x07ff) {
    b = c >> 6;
    *out++ = (b | 0xc0);
    b = c & 0x3f;
    *out++ = b | 0x80;
  } else if (0x0800 <= c && c <= 0xffff) {
    b = c >> 12;
    *out++ = (b | 0xe0);
    b = (c >> 6) & 0x3f;
    *out++ = b | 0x80;
    b = c & 0x3f;
    *out++ = b | 0x80;
  } else if (0x10000 <= c && c <= 0x10ffff) {
    b = c >> 18;
    *out++ = (b | 0xf0);
    b = (c >> 12) & 0x3f;
    *out++ = b | 0x80;
    b = (c >> 6) & 0x3f;
    *out++ = b | 0x80;
    b = c & 0x3f;
    *out++ = b | 0x80;
  }
}

// utility
bool is_basic_latin(uchar uc);
bool is_alnum(uchar uc);
bool is_hiragana(uchar uc);
bool is_katakana(uchar uc);
bool is_kanji(uchar uc);
bool is_hankaku(uchar uc);

std::string sanitize_utf8(const std::string& s);
ustring hankaku_to_zenkaku(const ustring& us);
ustring zenkaku_latin_to_basic_latin(const ustring& us);
ustring basic_latin_to_zenkaku_latin(const ustring& us);
uchar zenkaku_latin_to_basic_latin(uchar uc);
uchar basic_latin_to_zenkaku_latin(uchar uc);

std::ostream& operator<<(std::ostream& out, const ustring &str);
std::istream& operator>>(std::istream& in , ustring &str);

} // string
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_STRING_USTRING_H_
