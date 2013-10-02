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

#ifndef JUBATUS_UTIL_DATA_SERIALIZATION_STRING_H_
#define JUBATUS_UTIL_DATA_SERIALIZATION_STRING_H_

#include "base.h"

#include <string>

#include "reflect.h"

namespace jubatus {
namespace util{
namespace data{
namespace serialization{

template <class Archive, class CharType, class Traits, class Allocator>
void serialize(Archive &ar, std::basic_string<CharType, Traits, Allocator> &s)
{
  uint32_t length=static_cast<uint32_t>(s.length());
  ar & length;

  s.resize(length);
  for (uint32_t i=0;i<length;i++)
    ar & s[i];
}

class string_type : public type_rep {
public:
  string_type(){}

  void print(std::ostream &os){
    os<<"string";
  }
};

template <class CharType, class Traits, class Allocator>
inline void serialize(reflection &ref, std::basic_string<CharType, Traits, Allocator> &)
{
  ref.add("", jubatus::util::lang::shared_ptr<type_rep>(new string_type()));
}

} // serializatin
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SERIALIZATION_STRING_H_
