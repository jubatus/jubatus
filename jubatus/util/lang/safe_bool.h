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

#ifndef JUBATUS_UTIL_LANG_SAFE_BOOL_H_
#define JUBATUS_UTIL_LANG_SAFE_BOOL_H_

namespace jubatus {
namespace util{
namespace lang{

class safe_bool_base{
public:
  typedef void (safe_bool_base::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}

protected:
  safe_bool_base(){}
  safe_bool_base(const safe_bool_base &){}
  safe_bool_base& operator=(const safe_bool_base&){ return *this; }
  ~safe_bool_base(){}
};

template <class T>
class safe_bool : private safe_bool_base{
public:
  operator bool_type() const {
    return (static_cast<const T*>(this))->bool_test()
      ? &safe_bool_base::this_type_does_not_support_comparisons : 0;
  }
protected:
  ~safe_bool(){}
};

template <class T>
bool operator==(const safe_bool<T> &lhs, bool b)
{
  if (lhs)
    return b;
  else
    return !b;
}

template <class T>
bool operator==(bool b, const safe_bool<T> &rhs)
{
  if (rhs)
    return b;
  else
    return !b;
}

template <class T, class U>
void operator==(const safe_bool<T>& lhs, const safe_bool<U>&)
{
  lhs.this_type_does_not_support_comparisons();
}

template <class T, class U>
void operator!=(const safe_bool<T>& lhs, const safe_bool<U>&)
{
  lhs.this_type_does_not_support_comparisons();
}

} // lang
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_LANG_SAFE_BOOL_H_
