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

#ifndef JUBATUS_UTIL_TEXT_JSON_SERIALIZATION_H_
#define JUBATUS_UTIL_TEXT_JSON_SERIALIZATION_H_

#include "base.h"
#include "../../data/serialization.h"
#include "../../lang/safe_bool.h"

namespace jubatus {
namespace util {
namespace data {
namespace serialization {

class json_iarchive : public jubatus::util::lang::safe_bool<json_iarchive> {
public:
  json_iarchive(std::istream& is) : is(is) {}

  template <class T>
  void get(T& v) {
    is >> jubatus::util::text::json::via_json(v);
  }

private:
  std::istream& is;
};

template <class T>
json_iarchive& operator>>(json_iarchive& ar, T& v)
{
  ar.get(v);
  return ar;
}

template <class T>
inline json_iarchive& operator&(json_iarchive& ar, T& v)
{
  serialize<T>(ar, v);
  return ar;
}

template <class T>
inline void serialize(json_iarchive& ar, T& v)
{
  ar.get(v);
}

class json_oarchive : public jubatus::util::lang::safe_bool<json_oarchive> {
public:
  json_oarchive(std::ostream& os, bool pretty = false, bool escape = true)
    : os(os), pretty(pretty), escape(escape)
  {}

  template <class T>
  void put(const T& v) {
    jubatus::util::text::json::gen_print(os, jubatus::util::text::json::to_json(v), pretty, escape);
    if (!pretty)
      os << std::endl;
  }

private:
  std::ostream& os;
  bool pretty;
  bool escape;
};

template <class T>
json_oarchive& operator<<(json_oarchive& ar, const T& v)
{
  ar.put(v);
  return ar;
}

template <class T>
inline json_oarchive& operator&(json_oarchive& ar, T& v)
{
  serialize<T>(ar, v);
  return ar;
}

template <class T>
inline void serialize(json_oarchive& ar, T& v)
{
  ar.put(v);
}

} // serialization
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_TEXT_JSON_SERIALIZATION_H_
