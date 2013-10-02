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

#ifndef JUBATUS_UTIL_LANG_BIND_H_
#define JUBATUS_UTIL_LANG_BIND_H_

#include "mem_fn.h"

namespace jubatus {
namespace util{
namespace lang{

namespace _bi{

template <int I>
struct arg{};

template <class T>
struct type{};

template <class R, class F>
struct result_traits{
  typedef R type;
};

struct unspecified{};

template <class F>
struct result_traits<unspecified, F>{
  typedef typename F::result_type type;
};

// storage

template <class A1>
struct storage1{
  explicit storage1(A1 a1): a1(a1){}
  A1 a1;
};

template <int I>
struct storage1<arg<I> >{
  explicit storage1(arg<I>){}
  static arg<I> a1(){ return arg<I>(); }
};

#define DEF_STRAGE(CNUM, BNUM)						\
  template <TARG, class A##CNUM>					\
  struct storage##CNUM : public storage##BNUM<AARG>{			\
    explicit storage##CNUM(FARG, A##CNUM a##CNUM)			\
      : storage##BNUM<AARG>(RARG), a##CNUM(a##CNUM){}			\
    A##CNUM a##CNUM;							\
  };									\
  template <TARG, int I>						\
  struct storage##CNUM<AARG, arg<I> > : public storage##BNUM<AARG>{	\
    explicit storage##CNUM(FARG, arg<I>): storage##BNUM<AARG>(RARG){}	\
    static arg<I> a##CNUM(){ return arg<I>(); }				\
  };

#define TARG class A1
#define AARG A1
#define FARG A1 a1
#define RARG a1
DEF_STRAGE(2,1);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2
#define AARG A1, A2
#define FARG A1 a1, A2 a2
#define RARG a1, a2
DEF_STRAGE(3,2);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3
#define AARG A1, A2, A3
#define FARG A1 a1, A2 a2, A3 a3
#define RARG a1, a2, a3
DEF_STRAGE(4,3);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3, class A4
#define AARG A1, A2, A3, A4
#define FARG A1 a1, A2 a2, A3 a3, A4 a4
#define RARG a1, a2, a3, a4
DEF_STRAGE(5,4);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3, class A4, class A5
#define AARG A1, A2, A3, A4, A5
#define FARG A1 a1, A2 a2, A3 a3, A4 a4, A5 a5
#define RARG a1, a2, a3, a4, a5
DEF_STRAGE(6,5);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3, class A4, class A5, class A6
#define AARG A1, A2, A3, A4, A5, A6
#define FARG A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define RARG a1, a2, a3, a4, a5, a6
DEF_STRAGE(7,6);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3, class A4, class A5, class A6, class A7
#define AARG A1, A2, A3, A4, A5, A6, A7
#define FARG A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define RARG a1, a2, a3, a4, a5, a6, a7
DEF_STRAGE(8,7);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#define TARG class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define AARG A1, A2, A3, A4, A5, A6, A7, A8
#define FARG A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define RARG a1, a2, a3, a4, a5, a6, a7, a8
DEF_STRAGE(9,8);
#undef RARG
#undef FARG
#undef AARG
#undef TARG

#undef DEF_STRAGE

// list
class list0{
public:
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A&){
    return f();
  }
};

template <class A1>
class list1: private storage1<A1>{
private:
  typedef storage1<A1> base_type;
public:
  explicit list1(A1 a1): base_type(a1){}

  A1 operator[](arg<1>     ) const { return base_type::a1; }
  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1]);
  }
};

template <class A1, class A2>
class list2: private storage2<A1,A2>{
private:
  typedef storage2<A1,A2> base_type;
public:
  explicit list2(A1 a1, A2 a2): base_type(a1,a2){}

  A1 operator[](arg<1>     ) const { return base_type::a1; }
  A2 operator[](arg<2>     ) const { return base_type::a2; }
  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2]);
  }
};

template <class A1, class A2, class A3>
class list3: private storage3<A1,A2,A3>{
private:
  typedef storage3<A1,A2,A3> base_type;
public:
  explicit list3(A1 a1, A2 a2, A3 a3): base_type(a1,a2,a3){}

  A1 operator[](arg<1>     ) const { return base_type::a1; }
  A2 operator[](arg<2>     ) const { return base_type::a2; }
  A3 operator[](arg<3>     ) const { return base_type::a3; }
  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3]);
  }
};

template <class A1, class A2, class A3, class A4>
class list4: private storage4<A1,A2,A3,A4>{
private:
  typedef storage4<A1,A2,A3,A4> base_type;
public:
  explicit list4(A1 a1, A2 a2, A3 a3, A4 a4): base_type(a1,a2,a3,a4){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4]);
  }
};

template <class A1, class A2, class A3, class A4,class A5>
class list5: private storage5<A1,A2,A3,A4,A5>{
private:
  typedef storage5<A1,A2,A3,A4,A5> base_type;
public:
  explicit list5(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5): base_type(a1,a2,a3,a4,a5){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  A5 operator[](arg<5>(*)()) const { return base_type::a5; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4],
             a[base_type::a5]);
  }
};

template <class A1, class A2, class A3, class A4,class A5, class A6>
class list6: private storage6<A1,A2,A3,A4,A5,A6>{
private:
  typedef storage6<A1,A2,A3,A4,A5,A6> base_type;
public:
  explicit list6(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
    : base_type(a1,a2,a3,a4,a5,a6){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  A5 operator[](arg<5>(*)()) const { return base_type::a5; }
  A6 operator[](arg<6>(*)()) const { return base_type::a6; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4],
             a[base_type::a5],
             a[base_type::a6]);
  }
};

template <class A1, class A2, class A3, class A4,class A5, class A6, class A7>
class list7: private storage7<A1,A2,A3,A4,A5,A6,A7>{
private:
  typedef storage7<A1,A2,A3,A4,A5,A6,A7> base_type;
public:
  explicit list7(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
    : base_type(a1,a2,a3,a4,a5,a6,a7){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  A5 operator[](arg<5>(*)()) const { return base_type::a5; }
  A6 operator[](arg<6>(*)()) const { return base_type::a6; }
  A7 operator[](arg<7>(*)()) const { return base_type::a7; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4],
             a[base_type::a5],
             a[base_type::a6],
             a[base_type::a7]);
  }
};

template <class A1, class A2, class A3, class A4,class A5, class A6, class A7, class A8>
class list8: private storage8<A1,A2,A3,A4,A5,A6,A7,A8>{
private:
  typedef storage8<A1,A2,A3,A4,A5,A6,A7,A8> base_type;
public:
  explicit list8(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
    : base_type(a1,a2,a3,a4,a5,a6,a7,a8){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  A5 operator[](arg<5>(*)()) const { return base_type::a5; }
  A6 operator[](arg<6>(*)()) const { return base_type::a6; }
  A7 operator[](arg<7>(*)()) const { return base_type::a7; }
  A8 operator[](arg<8>(*)()) const { return base_type::a8; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4],
             a[base_type::a5],
             a[base_type::a6],
             a[base_type::a7],
             a[base_type::a8]);
  }
};

template <class A1, class A2, class A3, class A4,class A5, class A6, class A7, class A8, class A9>
class list9: private storage9<A1,A2,A3,A4,A5,A6,A7,A8,A9>{
private:
  typedef storage9<A1,A2,A3,A4,A5,A6,A7,A8,A9> base_type;
public:
  explicit list9(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
    : base_type(a1,a2,a3,a4,a5,a6,a7,a8,a9){}

  A1 operator[](arg<1>(*)()) const { return base_type::a1; }
  A2 operator[](arg<2>(*)()) const { return base_type::a2; }
  A3 operator[](arg<3>(*)()) const { return base_type::a3; }
  A4 operator[](arg<4>(*)()) const { return base_type::a4; }
  A5 operator[](arg<5>(*)()) const { return base_type::a5; }
  A6 operator[](arg<6>(*)()) const { return base_type::a6; }
  A7 operator[](arg<7>(*)()) const { return base_type::a7; }
  A8 operator[](arg<8>(*)()) const { return base_type::a8; }
  A9 operator[](arg<9>(*)()) const { return base_type::a9; }
  template <class T> T &operator[](T &v) const { return v; }

  template <class R, class F, class A>
  R operator()(type<R>, F &f, const A& a){
    return f(a[base_type::a1],
             a[base_type::a2],
             a[base_type::a3],
             a[base_type::a4],
             a[base_type::a5],
             a[base_type::a6],
             a[base_type::a7],
             a[base_type::a8],
             a[base_type::a9]);
  }
};

// bind_t
template <class R, class F, class L>
class bind_t{
public:
  typedef typename result_traits<R, F>::type result_type;

  bind_t(F f, const L &l): f(f), l(l) {}

#include "bind_template.h"

private:
  F f;
  L l;
};

} // _bi

// bind for function ptr

template <class R>
_bi::bind_t<R, R (*)(), _bi::list0>
bind(R (*f)())
{
  typedef R (*F)();
  typedef _bi::list0 L;
  return _bi::bind_t<R, F, L>(f, L());
}

template <class R, class A1, class B1>
_bi::bind_t<R, R (*)(B1), _bi::list1<A1> >
bind(R (*f)(B1), A1 a1)
{
  typedef R (*F)(B1);
  typedef _bi::list1<A1> L;
  return _bi::bind_t<R, F, L>(f, L(a1));
}

template <class R, class A1, class A2, class B1, class B2>
_bi::bind_t<R, R (*)(B1,B2), _bi::list2<A1,A2> >
bind(R (*f)(B1,B2), A1 a1, A2 a2)
{
  typedef R (*F)(B1,B2);
  typedef _bi::list2<A1,A2> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2));
}

template <class R, class A1, class A2, class A3, class B1, class B2, class B3>
_bi::bind_t<R, R (*)(B1,B2,B3), _bi::list3<A1,A2,A3> >
bind(R (*f)(B1,B2,B3), A1 a1, A2 a2, A3 a3)
{
  typedef R (*F)(B1,B2,B3);
  typedef _bi::list3<A1,A2,A3> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3));
}

template <class R,
          class A1, class A2, class A3, class A4,
          class B1, class B2, class B3, class B4>
_bi::bind_t<R, R (*)(B1,B2,B3,B4), _bi::list4<A1,A2,A3,A4> >
bind(R (*f)(B1,B2,B3,B4), A1 a1, A2 a2, A3 a3, A4 a4)
{
  typedef R (*F)(B1,B2,B3,B4);
  typedef _bi::list4<A1,A2,A3,A4> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4));
}

template <class R,
          class A1, class A2, class A3, class A4, class A5,
          class B1, class B2, class B3, class B4, class B5>
_bi::bind_t<R, R (*)(B1,B2,B3,B4,B5), _bi::list5<A1,A2,A3,A4,A5> >
bind(R (*f)(B1,B2,B3,B4,B5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  typedef R (*F)(B1,B2,B3,B4,B5);
  typedef _bi::list5<A1,A2,A3,A4,A5> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4,a5));
}

template <class R,
          class A1, class A2, class A3, class A4, class A5, class A6,
          class B1, class B2, class B3, class B4, class B5, class B6>
_bi::bind_t<R, R (*)(B1,B2,B3,B4,B5,B6), _bi::list6<A1,A2,A3,A4,A5,A6> >
bind(R (*f)(B1,B2,B3,B4,B5,B6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  typedef R (*F)(B1,B2,B3,B4,B5,B6);
  typedef _bi::list6<A1,A2,A3,A4,A5,A6> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4,a5,a6));
}

template <class R,
          class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class B1, class B2, class B3, class B4, class B5, class B6, class B7>
_bi::bind_t<R, R (*)(B1,B2,B3,B4,B5,B6,B7), _bi::list7<A1,A2,A3,A4,A5,A6,A7> >
bind(R (*f)(B1,B2,B3,B4,B5,B6,B7), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
  typedef R (*F)(B1,B2,B3,B4,B5,B6,B7);
  typedef _bi::list7<A1,A2,A3,A4,A5,A6,A7> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7));
}

template <class R,
          class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8,
          class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8>
_bi::bind_t<R, R (*)(B1,B2,B3,B4,B5,B6,B7,B8), _bi::list8<A1,A2,A3,A4,A5,A6,A7,A8> >
bind(R (*f)(B1,B2,B3,B4,B5,B6,B7,B8), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
{
  typedef R (*F)(B1,B2,B3,B4,B5,B6,B7,B8);
  typedef _bi::list8<A1,A2,A3,A4,A5,A6,A7,A8> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7,a8));
}

template <class R,
          class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9,
          class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9>
_bi::bind_t<R, R (*)(B1,B2,B3,B4,B5,B6,B7,B8,B9), _bi::list9<A1,A2,A3,A4,A5,A6,A7,A8,A9> >
bind(R (*f)(B1,B2,B3,B4,B5,B6,B7,B8,B9), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
{
  typedef R (*F)(B1,B2,B3,B4,B5,B6,B7,B8,B9);
  typedef _bi::list9<A1,A2,A3,A4,A5,A6,A7,A8,A9> L;
  return _bi::bind_t<R, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7,a8,a9));
}

// bind for function object

template<class F>
_bi::bind_t<_bi::unspecified, F, _bi::list0>
bind(F f)
{
  typedef _bi::list0 L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L());
}

template<class F, class A1>
_bi::bind_t<_bi::unspecified, F, _bi::list1<A1> >
bind(F f, A1 a1)
{
  typedef _bi::list1<A1> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1));
}

template<class F, class A1, class A2>
_bi::bind_t<_bi::unspecified, F, _bi::list2<A1,A2> >
bind(F f, A1 a1, A2 a2)
{
  typedef _bi::list2<A1,A2> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2));
}

template<class F, class A1, class A2, class A3>
_bi::bind_t<_bi::unspecified, F, _bi::list3<A1,A2,A3> >
bind(F f, A1 a1, A2 a2, A3 a3)
{
  typedef _bi::list3<A1,A2,A3> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3));
}

template<class F, class A1, class A2, class A3, class A4>
_bi::bind_t<_bi::unspecified, F, _bi::list4<A1,A2,A3,A4> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4)
{
  typedef _bi::list4<A1,A2,A3,A4> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4));
}

template<class F, class A1, class A2, class A3, class A4, class A5>
_bi::bind_t<_bi::unspecified, F, _bi::list5<A1,A2,A3,A4,A5> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  typedef _bi::list5<A1,A2,A3,A4,A5> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4,a5));
}

template<class F, class A1, class A2, class A3, class A4, class A5, class A6>
_bi::bind_t<_bi::unspecified, F, _bi::list6<A1,A2,A3,A4,A5,A6> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  typedef _bi::list6<A1,A2,A3,A4,A5,A6> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4,a5,a6));
}

template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
_bi::bind_t<_bi::unspecified, F, _bi::list7<A1,A2,A3,A4,A5,A6,A7> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
  typedef _bi::list7<A1,A2,A3,A4,A5,A6,A7> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7));
}

template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
_bi::bind_t<_bi::unspecified, F, _bi::list8<A1,A2,A3,A4,A5,A6,A7,A8> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
{
  typedef _bi::list8<A1,A2,A3,A4,A5,A6,A7,A8> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7,a8));
}

template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
_bi::bind_t<_bi::unspecified, F, _bi::list9<A1,A2,A3,A4,A5,A6,A7,A8,A9> >
bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
{
  typedef _bi::list9<A1,A2,A3,A4,A5,A6,A7,A8,A9> L;
  return _bi::bind_t<_bi::unspecified, F, L>(f, L(a1,a2,a3,a4,a5,a6,a7,a8,a9));
}

// bind for member function ptr

#define INSIDE_BIND

#define MF_NAME(x) x
#define MF_CC
#include "bind_mf.h"
#undef MF_CC
#undef MF_NAME

#undef INSIDE_BIND

// placeholders

_bi::arg<1> const _1 = {};
_bi::arg<2> const _2 = {};
_bi::arg<3> const _3 = {};
_bi::arg<4> const _4 = {};
_bi::arg<5> const _5 = {};
_bi::arg<6> const _6 = {};
_bi::arg<7> const _7 = {};
_bi::arg<8> const _8 = {};
_bi::arg<9> const _9 = {};

} // lang
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_LANG_BIND_H_
