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

#define INSIDE_MEM_FN_DEF

#define NNAME MF_NAME(mf0)
#define CNAME MF_NAME(cmf0)
#define TARG
#define UARG
#define AARG
#define FARG
#define GARG
#define RARG
#define SARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf1)
#define CNAME MF_NAME(cmf1)
#define TARG , class A1
#define UARG , class B1
#define AARG A1
#define FARG , A1 a1
#define GARG , B1 a1
#define RARG a1
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf2)
#define CNAME MF_NAME(cmf2)
#define TARG , class A1, class A2
#define UARG , class B1, class B2
#define AARG A1, A2
#define FARG , A1 a1, A2 a2
#define GARG , B1 a1, B2 a2
#define RARG a1, a2
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf3)
#define CNAME MF_NAME(cmf3)
#define TARG , class A1, class A2, class A3
#define UARG , class B1, class B2, class B3
#define AARG A1, A2, A3
#define FARG , A1 a1, A2 a2, A3 a3
#define GARG , B1 a1, B2 a2, B3 a3
#define RARG a1, a2, a3
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf4)
#define CNAME MF_NAME(cmf4)
#define TARG , class A1, class A2, class A3, class A4
#define UARG , class B1, class B2, class B3, class B4
#define AARG A1, A2, A3, A4
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4
#define RARG a1, a2, a3, a4
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf5)
#define CNAME MF_NAME(cmf5)
#define TARG , class A1, class A2, class A3, class A4, class A5
#define UARG , class B1, class B2, class B3, class B4, class B5
#define AARG A1, A2, A3, A4, A5
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4, A5 a5
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4, B5 a5
#define RARG a1, a2, a3, a4, a5
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf6)
#define CNAME MF_NAME(cmf6)
#define TARG , class A1, class A2, class A3, class A4, class A5, class A6
#define UARG , class B1, class B2, class B3, class B4, class B5, class B6
#define AARG A1, A2, A3, A4, A5, A6
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4, B5 a5, B6 a6
#define RARG a1, a2, a3, a4, a5, a6
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf7)
#define CNAME MF_NAME(cmf7)
#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7
#define UARG , class B1, class B2, class B3, class B4, class B5, class B6, class B7
#define AARG A1, A2, A3, A4, A5, A6, A7
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4, B5 a5, B6 a6, B7 a7
#define RARG a1, a2, a3, a4, a5, a6, a7
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf8)
#define CNAME MF_NAME(cmf8)
#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define UARG , class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8
#define AARG A1, A2, A3, A4, A5, A6, A7, A8
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4, B5 a5, B6 a6, B7 a7, B8 a8
#define RARG a1, a2, a3, a4, a5, a6, a7, a8
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#define NNAME MF_NAME(mf9)
#define CNAME MF_NAME(cmf9)
#define TARG , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9
#define UARG , class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9
#define AARG A1, A2, A3, A4, A5, A6, A7, A8, A9
#define FARG , A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9
#define GARG , B1 a1, B2 a2, B3 a3, B4 a4, B5 a5, B6 a6, B7 a7, B8 a8, B9 a9
#define RARG a1, a2, a3, a4, a5, a6, a7, a8, a9
#define SARG , RARG
#include "mem_fn_tmpl.h"
#undef SARG
#undef RARG
#undef GARG
#undef FARG
#undef AARG
#undef UARG
#undef TARG
#undef CNAME
#undef NNAME

#undef INSIDE_MEM_FN_DEF

#endif // INSIDE_MEM_FM
