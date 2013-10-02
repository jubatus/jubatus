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

#include "ustring.h"

#include <stddef.h>
#include <string.h>
#include <iterator>

using namespace std;

namespace pfi {
namespace data {
namespace string {

#define UTF_ZENKAKU_BAR 0x30fc // ー
#define UTF_HANKAKU_BAR 0x002d // -
#define UTF_ZENKAKU_SPACE 0x3000
#define UTF_HANKAKU_SPACE 0x0020
#define UTF_HANKAKU_DAKUTEN 0xff9e 
#define UTF_ZENKAKU_DAKUTEN 0x3099 
#define UTF_HANKAKU_HANDAKUTEN 0xff9f 
#define UTF_ZENKAKU_HANDAKUTEN 0x309a 

static int hankaku_zenkaku_tbl[]={
  0x0000,0x3002,0x300c,0x300d,0x3001,0x30fb,0x30f2,0x30a1,0x30a3,0x30a5,0x30a7,0x30a9,0x30e3,0x30e5,0x30e7,0x30c3, // 0xff60 - 0xff6f
  0x30fc,0x30a2,0x30a4,0x30a6,0x30a8,0x30aa,0x30ab,0x30ad,0x30af,0x30b1,0x30b3,0x30b5,0x30b7,0x30b9,0x30bb,0x30bd, // 0xff70 - 0xff7f
  0x30bf,0x30c1,0x30c4,0x30c6,0x30c8,0x30ca,0x30cb,0x30cc,0x30cd,0x30ce,0x30cf,0x30d2,0x30d5,0x30d8,0x30db,0x30de, // 0xff80 - 0xff8f
  0x30df,0x30e0,0x30e1,0x30e2,0x30e4,0x30e6,0x30e8,0x30e9,0x30ea,0x30eb,0x30ec,0x30ed,0x30ef,0x30f3,0x3099,0x309a // 0xff90 - 0x9f
};

static int take_dakuten_tbl[]={
  //  ァアィイゥウェエォオカガキギク
  0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,1,
  //グケゲコゴサザシジスズセゼソゾタ
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  //ダチヂッツヅテデトドナニヌネノハ
  0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,1,
  //バパヒビピフブプヘベペホボポマミ
  0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,
  //ムメモャヤュユョヨラリルレロヮワ
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  //
  0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static int take_handakuten_tbl[]={
  //  ァアィイゥウェエォオカガキギク
  0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,
  //グケゲコゴサザシジスズセゼソゾタ
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  //ダチヂッツヅテデトドナニヌネノハ
  0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,1,
  //バパヒビピフブプヘベペホボポマミ
  0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,
  //ムメモャヤュユョヨラリルレロヮワ
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  //
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

ustring string_to_ustring(const char* p) {
  const char* const end = p + strlen(p);
  ustring res;
  while(*p) res+=chars_to_uchar(p, end);
  return res;
}

ustring string_to_ustring(const std::string& s) {
  return string_to_ustring(s.c_str());
}

std::string ustring_to_string(const ustring& us) {
  std::string res;
  std::back_insert_iterator<std::string> ins(res);
  for(size_t i = 0 ; i < us.size() ; i ++)
    uchar_to_chars(us[i], ins);
  return res;
}

std::string uchar_to_string(uchar c){
  std::string res;
  std::back_insert_iterator<std::string> ins(res);
  uchar_to_chars(c, ins);
  return res;
}

uchar string_to_uchar(const char* p) {
  size_t n = 0;
  while (p[n] && n < 6)
    ++n;
  return chars_to_uchar(p, p+n);
}

uchar string_to_uchar(const std::string& s) {
  std::string::const_iterator it = s.begin();
  return chars_to_uchar(it, s.end());
}

bool is_basic_latin(uchar uc) {
  return uc<=0x007f;
}
bool is_alnum(uchar uc) {
  return uc<=0x007f&&isalnum((char)(uc));
}
bool is_hiragana(uchar uc) {
  return (0x3040<=uc&&uc<=0x309f); //||uc==UTF_ZENKAKU_BAR;
}
bool is_katakana(uchar uc) {
  return 0x30a0<=uc&&uc<=0x30ff;
}
bool is_kanji(uchar uc) {
  return 0x4e00<=uc&&uc<=0x9fff;
}
bool is_hankaku(uchar uc) {
  return 0xff60<=uc&&uc<=0xff9f;
}

namespace {
inline bool utf8_invalid2(unsigned char* p){
  return *p < 0xC2 || (p[1] & 0x80) == 0 || (p[1] & 0xC0) == 0xC0;
}

inline bool utf8_invalid3(unsigned char* p){
  return (p[2] & 0x80) == 0
    || ((*p) == 0xEF && p[1] == 0xBF ? p[2] > 0xBD : (p[2] & 0xC0) == 0xC0)
    ||   (*p == 0xE0 ? p[1] < 0xA0
      || (p[1] & 0xC0) == 0xC0 : (p[1] & 0x80) == 0
      || (*p == 0xED ? p[1] > 0x9F : (p[1] & 0xC0) == 0xC0));
}

inline bool utf8_invalid4(unsigned char* p){
  return (p[3] & 0x80) == 0 || (p[3] & 0xC0) == 0xC0 || (p[2] & 0x80) == 0 || (p[2] & 0xC0) == 0xC0
    || (*p == 0xF0 ? p[1] < 0x90 || (p[1] & 0xC0) == 0xC0 : (p[1] & 0x80) == 0 || 
        (*p == 0xF4 ? p[1] > 0x8F : (p[1] & 0xC0) == 0xC0));
}
  
int utf8_skip(const std::string& s, size_t p){
  if (s[p] == 0) return -1;
  if ((s[p] & 0x80) == 0x00) return 0;
  else if ((s[p]&0xc0) == 0x80) return -1;
  else if ((s[p]&0xe0) == 0xc0) return 1;
  else if ((s[p]&0xf0) == 0xe0) return 2;
  else if ((s[p]&0xf8) == 0xf0) return 3;
  return -1;
}
} // namespace

std::string sanitize_utf8(const std::string& s){
  std::string ret;
  unsigned char buf[10];
  for (size_t p = 0; p < s.size();) {
    const int ccc = utf8_skip(s, p);
    if (ccc < 0) {
      p++;
      continue;
    }
    size_t orgp = p;
    p++;
    int i = 0;
    buf[i] = s[orgp];
    for (; p <= ccc + orgp && p < s.size(); p++) {
      if ((s[p]&0xc0) != 0x80) break;
    }
    int cccc = ccc + 1;
    std::string retpartial;
    if (p == ccc + orgp + 1) {
      for (size_t t = orgp; t < p; t++) {
        retpartial += s[t];
        buf[i] = s[t];
        i++;
      }
    } else {
      continue;
    }
    bool invalid = true;
    if (cccc == 1) {
      if (buf[0] < 0x20) continue;
      invalid = false;
    }
    if (cccc == 2) {
      if (utf8_invalid2(buf)) continue;
      if (0xc2 <= buf[0] && buf[0] <= 0xdf && 0x80 <= buf[1] && buf[1] <= 0xbf) invalid = false;
    }
    if (cccc == 3) {
      if (utf8_invalid3(buf)) continue;
      if (buf[0] == 0xe0 && 0xa0 <= buf[1] && buf[1] <= 0xbf && 0x80 <= buf[2] && buf[2] <= 0xbf) invalid = false;
      if (0xe1 <= buf[0] && buf[0] <= 0xec && 0x80 <= buf[1] && buf[1] <= 0xbf && 0x80 <= buf[2] && buf[2] <= 0xbf) invalid = false;
      if (buf[0] == 0xed && 0x80 <= buf[1] && buf[1] <= 0x9f && 0x80 <= buf[2] && buf[2] <= 0xbf) invalid = false;
      if (0xee <= buf[0] && buf[0] <= 0xef && 0x80 <= buf[1] && buf[1] <= 0xbf && 0x80 <= buf[2] && buf[2] <= 0xbf) invalid = false;
    }
    if (cccc == 4) {
      if (utf8_invalid4(buf)) continue;
      if (buf[0] == 0xf0 && 0x90 <= buf[1] && buf[1] <= 0xbf && 0x80 <= buf[2] && buf[2] <= 0xbf && 0x80 <= buf[3] && buf[3] <= 0xbf) invalid = false;

      if (0xf1 <= buf[0] && buf[0] <= 0xf3 && 0x80 <= buf[1] && buf[1] <= 0xbf && 0x80 <= buf[2] && buf[2] <= 0xbf && 0x80 <= buf[3] && buf[3] <=
          0xbf) invalid = false;
      if (buf[0] == 0xf4 && 0x80 <= buf[1] && buf[1] <= 0x8f && 0x80 <= buf[2] && buf[2] <= 0xbf && 0x80 <= buf[3] && buf[3] <= 0xbf) invalid = false;
    }
    // TODO: check cccc == 5, cccc == 6
    if (invalid) continue;
    ret += retpartial;
  }
  return ret;
}

ustring hankaku_to_zenkaku(const ustring& ustr){
  ustring res;
  for (size_t i=0;i<ustr.size();++i) {
    uchar c=res.size()>0?res[res.size()-1]:0xFFFFFFFFU;
    if (is_hankaku(ustr[i])) {
      if (ustr[i]==UTF_HANKAKU_DAKUTEN) { 
        if (c!=0xFFFFFFFFU&&is_katakana(c)&&take_dakuten_tbl[c-0x30a0]) {
          if (c==0x30a6) { // ウ
            res[res.size()-1]=0x30f4;
          } else if (c==0x30f2) { // ヲ
            res[res.size()-1]=0x30fa;
          } else {
            ++res[res.size()-1];
          }
        } else {
          res.push_back(UTF_ZENKAKU_DAKUTEN); // zenkaku dakuten
        }
      } else if (ustr[i]==UTF_HANKAKU_HANDAKUTEN) {
        if (c!=0xFFFFFFFFU&&is_katakana(c)&&take_handakuten_tbl[c-0x30a0]) {
          res[res.size()-1]+=2;
        } else {
          res.push_back(UTF_ZENKAKU_HANDAKUTEN); // zenkaku dakuten
        }
      } else {
        res.push_back(hankaku_zenkaku_tbl[ustr[i]-0xff60]);
      }
    } else if (ustr[i]==UTF_HANKAKU_BAR) { // -
      if (c!=0xFFFFFFFFU&&!is_basic_latin(c)) {
        res.push_back(UTF_ZENKAKU_BAR);
      } else {
        res.push_back(ustr[i]);
      }
    } else {
      res.push_back(ustr[i]);
    }
  } 
  return res;
}

namespace {
inline uchar zenkaku_latin_to_basic_latin_impl(uchar uc)
{
  if (0xff01<=uc&&uc<=0xff5e)
    return uc-0xff00+0x0020;
  else if (uc==UTF_ZENKAKU_SPACE)
    return UTF_HANKAKU_SPACE;
  else
    return uc;
}

inline uchar basic_latin_to_zenkaku_latin_impl(uchar uc)
{
  if (0x0021<=uc&&uc<=0x007e)
    return uc-0x0020+0xff00;
  else if (uc==UTF_HANKAKU_SPACE)
    return UTF_ZENKAKU_SPACE;
  else
    return uc;
}
} // namespace

ustring zenkaku_latin_to_basic_latin(const ustring& us)
{
  ustring res=us;
  for (size_t i=0;i<us.size();++i) {
    res[i]=zenkaku_latin_to_basic_latin_impl(us[i]);
  }
  return res;
}

ustring basic_latin_to_zenkaku_latin(const ustring& us)
{
  ustring res=us;
  for (size_t i=0;i<us.size();++i) {
    res[i]=basic_latin_to_zenkaku_latin_impl(us[i]);
  }
  return res;
}

uchar zenkaku_latin_to_basic_latin(uchar uc)
{
  return zenkaku_latin_to_basic_latin_impl(uc);
}

uchar basic_latin_to_zenkaku_latin(uchar uc)
{
  return basic_latin_to_zenkaku_latin_impl(uc);
}

ostream& operator<<(ostream& out, const ustring& us) {
  ostreambuf_iterator<char> it(out);
  for (size_t i=0; i<us.size(); i++)
    uchar_to_chars(us[i], it);
  return out;
}

istream& operator>>(istream& in, ustring& us) {
  std::string s;
  in>>s;
  us=string_to_ustring(s);
  return in;
}

} // string
} // data
} // pfi
