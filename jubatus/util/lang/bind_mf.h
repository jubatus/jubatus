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

// *** THIS FILE MUST NOT BE INCLUDE DIRECTORY ***
// THIS FILE IS INCLUDED BY "bind.h"

#ifdef INSIDE_BIND

template <class R, class T, class A1>
_bi::bind_t<R, _mfi::MF_NAME(mf0)<R,T>, _bi::list1<A1> >
bind(R (MF_CC T::*f)(), A1 a1)
{
  typedef _mfi::MF_NAME(mf0)<R,T> F;
  typedef typename _bi::list1<A1> L;
  return _bi::bind_t<R, F, L>(mem_fn(f), L(a1));
}

template <class R, class T, class A1>
_bi::bind_t<R, const _mfi::MF_NAME(cmf0)<R,T>, _bi::list1<A1> >
bind(R (MF_CC T::*f)() const, A1 a1)
{
  typedef const _mfi::MF_NAME(cmf0)<R,T> F;
  typedef typename _bi::list1<A1> L;
  return _bi::bind_t<R, F, L>(mem_fn(f), L(a1));
}

#define DEF_BIND(ANUM, CNUM)						\
  template <class R, class T, class A1					\
            AARG BARG >							\
  _bi::bind_t<R, _mfi::MF_NAME(mf##ANUM)<R,T BTYPES>, _bi::list##CNUM<A1 ATYPES> > \
  bind(R (MF_CC T::*f)(BTYPESN), A1 a1 FARG)				\
  {									\
    typedef _mfi::MF_NAME(mf##ANUM)<R,T BTYPES> F;			\
    typedef typename _bi::list##CNUM<A1 ATYPES> L;			\
    return _bi::bind_t<R, F, L>(mem_fn(f), L(a1 RARG));			\
  }									\
  template <class R, class T, class A1					\
            AARG BARG >							\
  _bi::bind_t<R, const _mfi::MF_NAME(cmf##ANUM)<R,T BTYPES>, _bi::list##CNUM<A1 ATYPES> > \
  bind(R (MF_CC T::*f)(BTYPESN) const, A1 a1 FARG)			\
  {									\
    typedef const _mfi::MF_NAME(cmf##ANUM)<R,T BTYPES> F;		\
    typedef typename _bi::list##CNUM<A1 ATYPES> L;		\
    return _bi::bind_t<R, F, L>(mem_fn(f), L(a1 RARG));			\
  }

#define AARG , class A2
#define BARG , class B2
#define ATYPES ,A2
#define BTYPESN B2
#define BTYPES , BTYPESN
#define FARG , A2 a2
#define RARG , a2
DEF_BIND(1,2)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3
#define BARG , class B2, class B3
#define ATYPES ,A2, A3
#define BTYPESN B2, B3
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3
#define RARG , a2, a3
DEF_BIND(2,3)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4
#define BARG , class B2, class B3, class B4
#define ATYPES ,A2, A3, A4
#define BTYPESN B2, B3, B4
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4
#define RARG , a2, a3, a4
DEF_BIND(3,4)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4, class A5
#define BARG , class B2, class B3, class B4, class B5
#define ATYPES ,A2, A3, A4, A5
#define BTYPESN B2, B3, B4, B5
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4, A5 a5
#define RARG , a2, a3, a4, a5
DEF_BIND(4,5)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4, class A5, class A6
#define BARG , class B2, class B3, class B4, class B5, class B6
#define ATYPES ,A2, A3, A4, A5, A6
#define BTYPESN B2, B3, B4, B5, B6
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define RARG , a2, a3, a4, a5, a6
DEF_BIND(5,6)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4, class A5, class A6, class A7
#define BARG , class B2, class B3, class B4, class B5, class B6, class B7
#define ATYPES ,A2, A3, A4, A5, A6, A7
#define BTYPESN B2, B3, B4, B5, B6, B7
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define RARG , a2, a3, a4, a5, a6, a7
DEF_BIND(6,7)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define BARG , class B2, class B3, class B4, class B5, class B6, class B7, class B8
#define ATYPES ,A2, A3, A4, A5, A6, A7, A8
#define BTYPESN B2, B3, B4, B5, B6, B7, B8
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define RARG , a2, a3, a4, a5, a6, a7, a8
DEF_BIND(7,8)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#define AARG , class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9
#define BARG , class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9
#define ATYPES ,A2, A3, A4, A5, A6, A7, A8, A9
#define BTYPESN B2, B3, B4, B5, B6, B7, B8, B9
#define BTYPES , BTYPESN
#define FARG , A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9
#define RARG , a2, a3, a4, a5, a6, a7, a8, a9
DEF_BIND(8,9)
#undef RARG
#undef FARG
#undef BTYPES
#undef BTYPESN
#undef ATYPES
#undef BARG
#undef AARG

#undef DEF_BIND

#endif
