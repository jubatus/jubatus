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

#ifndef INCLUDE_GUARD_PFI_LANG_WEAK_PTR_H_
#define INCLUDE_GUARD_PFI_LANG_WEAK_PTR_H_

#include <memory>
#include <tr1/memory>

namespace pfi {
namespace lang {

template <class T, class TM>
class shared_ptr;

template <class T>
class weak_ptr : public std::tr1::weak_ptr<T> {
  typedef std::tr1::weak_ptr<T> base;

public:
  weak_ptr() {}

  template <class U, class UM>
  weak_ptr(const shared_ptr<U, UM>& p) : base(p) {}

  template <class U>
  weak_ptr(const weak_ptr<U>& p) : base(p) {}

  template <class U>
  weak_ptr& operator=(const weak_ptr<U>& p) {
    base::operator=(p);
    return *this;
  }

  template <class U, class UM>
  weak_ptr& operator=(const shared_ptr<U, UM>& p) {
    base::operator=(p);
    return *this;
  }

  shared_ptr<T> lock() const {
    return shared_ptr<T>(base::lock());
  }
};

} // lang
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_LANG_WEAK_PTR_H_
