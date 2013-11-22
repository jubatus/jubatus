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

// *** THIS FILE MUST NOT INCLUDE DIRECTORY ***
// THIS FILE IS INCLUDED BY "mem_fn_tmpl.h"

#ifdef INSIDE_MEM_FN_DEF

template <class R, class T TARG>
class NNAME{
private:
  typedef R (MF_CC T::*F)(AARG);

public:
  typedef R result_type;
  typedef T *argument_type;

  explicit NNAME(F f): f(f){}

  R operator()(T *p FARG) const{
    return (p->*f)(RARG);
  }

  template <class U UARG>
  R operator()(U &u GARG) const{
    return call(u,&u SARG);
  }

  R operator()(T &t FARG) const{
    return (t.*f)(RARG);
  }

  bool operator==(const NNAME &r) const{
    return f==r.f;
  }
  bool operator!=(const NNAME &r) const{
    return f!=r.f;
  }

private:
  template <class U UARG>
  R call(U &u, const T * GARG) const{
    return (u.*f)(RARG);
  }
  template <class U UARG>
  R call(U &u, const void * GARG) const{
    return (get_pointer(u)->*f)(RARG);
  }

  F f;
};

template <class R, class T TARG>
class CNAME{
private:
  typedef R (MF_CC T::*F)(AARG) const;

public:
  typedef R result_type;
  typedef T const *argument_type;

  explicit CNAME(F f): f(f){}

  R operator()(T *p FARG) const{
    return (p->*f)(RARG);
  }

  template <class U UARG>
  R operator()(U &u GARG) const{
    return call(u,&u SARG);
  }

  R operator()(T &t FARG) const{
    return (t.*f)(RARG);
  }

  bool operator==(const CNAME &r) const{
    return f==r.f;
  }
  bool operator!=(const CNAME &r) const{
    return f!=r.f;
  }

private:
  template <class U UARG>
  R call(U &u, const T * GARG) const{
    return (u.*f)(RARG);
  }

  template <class U UARG>
  R call(U &u, const void * GARG) const{
    return (get_pointer(u)->*f)(RARG);
  }

  F f;
};


#endif // INSIDE_MEM_FN_DEF
