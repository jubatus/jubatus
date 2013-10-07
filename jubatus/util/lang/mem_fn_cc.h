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
// THIS FILE IS INCLUDED BY "mem_fn.h"

#ifdef INSIDE_MEM_FN

#define _mem_fn_def(name)				\
  template <class R, class T TARG>			\
  _mfi::MF_NAME(name)<R,T BARG>				\
  mem_fn(R (MF_CC T::*f)(AARG)){			\
    return _mfi::MF_NAME(name)<R,T BARG>(f);		\
  }							\
  template <class R, class T TARG>			\
  _mfi::MF_NAME(c##name)<R,T BARG>			\
    mem_fn(R (MF_CC T::*f)(AARG) const){		\
    return _mfi::MF_NAME(c##name)<R,T BARG>(f);		\
  }

#define TARG
#define AARG
#define BARG
_mem_fn_def(mf0)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1
#define AARG A1
#define BARG , AARG
_mem_fn_def(mf1)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2
#define AARG A1, A2
#define BARG , AARG
_mem_fn_def(mf2)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3
#define AARG A1, A2, A3
#define BARG , AARG
_mem_fn_def(mf3)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4
#define AARG A1, A2, A3, A4
#define BARG , AARG
_mem_fn_def(mf4)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4, class A5
#define AARG A1, A2, A3, A4, A5
#define BARG , AARG
_mem_fn_def(mf5)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4, class A5, class A6
#define AARG A1, A2, A3, A4, A5, A6
#define BARG , AARG
_mem_fn_def(mf6)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7
#define AARG A1, A2, A3, A4, A5, A6, A7
#define BARG , AARG
_mem_fn_def(mf7)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define AARG A1, A2, A3, A4, A5, A6, A7, A8
#define BARG , AARG
_mem_fn_def(mf8)
#undef BARG
#undef AARG
#undef TARG

#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9
#define AARG A1, A2, A3, A4, A5, A6, A7, A8, A9
#define BARG , AARG
_mem_fn_def(mf9)
#undef BARG
#undef AARG
#undef TARG

#undef _mem_fn_def

#endif // INSIDE_MEM_FN
