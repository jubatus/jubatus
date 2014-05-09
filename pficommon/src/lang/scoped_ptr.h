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

#ifndef INCLUDE_GUARD_PFI_LANG_SCOPED_PTR_H_
#define INCLUDE_GUARD_PFI_LANG_SCOPED_PTR_H_

#include <algorithm>
#include "safe_bool.h"

namespace pfi{
namespace lang{

template <class T>
class scoped_ptr : public safe_bool<scoped_ptr<T> > {
public:
  explicit scoped_ptr(T* p = 0) : p_(p) {}
  ~scoped_ptr(){ delete p_; }

  bool bool_test() const { return p_; }

  T& operator*() const { return *get(); }
  T* operator->() const { return get(); }
  T* get() const { return p_; }

  void reset(T* p = 0) {
      scoped_ptr tmp(p);
      swap(tmp);
  }
  void swap(scoped_ptr<T>& p) {
      std::swap(p_, p.p_);
  }

private:
  scoped_ptr(scoped_ptr const&);
  scoped_ptr& operator=(scoped_ptr const&);

  T* p_;
};

template <typename T>
void swap(scoped_ptr<T>& x, scoped_ptr<T>& y)
{
    x.swap(y);
}

} // lang
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_LANG_SCOPED_PTR_H_
