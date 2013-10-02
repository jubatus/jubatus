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

// Do not include this header directory.
// This file is included by bind.h.

  result_type operator()() {
    return l(type<result_type>(), f, list0());
  }

  template <class A1>
  result_type operator()(A1 &a1) {
    return l(type<result_type>(), f, list1<A1&>(a1));
  }

  template <class A1>
  result_type operator()(const A1 &a1) {
    return l(type<result_type>(), f, list1<const A1&>(a1));
  }

  template <class A1, class A2>
  result_type operator()(A1 &a1, A2 &a2) {
    return l(type<result_type>(), f, list2<A1&, A2&>(a1, a2));
  }

  template <class A1, class A2>
  result_type operator()(const A1 &a1, A2 &a2) {
    return l(type<result_type>(), f, list2<const A1&, A2&>(a1, a2));
  }

  template <class A1, class A2>
  result_type operator()(A1 &a1, const A2 &a2) {
    return l(type<result_type>(), f, list2<A1&, const A2&>(a1, a2));
  }

  template <class A1, class A2>
  result_type operator()(const A1 &a1, const A2 &a2) {
    return l(type<result_type>(), f, list2<const A1&, const A2&>(a1, a2));
  }

  template <class A1, class A2, class A3>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3) {
    return l(type<result_type>(), f, list3<A1&, A2&, A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3) {
    return l(type<result_type>(), f, list3<const A1&, A2&, A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3) {
    return l(type<result_type>(), f, list3<A1&, const A2&, A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3) {
    return l(type<result_type>(), f, list3<const A1&, const A2&, A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3) {
    return l(type<result_type>(), f, list3<A1&, A2&, const A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3) {
    return l(type<result_type>(), f, list3<const A1&, A2&, const A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3) {
    return l(type<result_type>(), f, list3<A1&, const A2&, const A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3) {
    return l(type<result_type>(), f, list3<const A1&, const A2&, const A3&>(a1, a2, a3));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, A2&, A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, A2&, A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, const A2&, A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, const A2&, A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, A2&, const A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, A2&, const A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, const A2&, const A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, const A2&, const A3&, A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, A2&, A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, A2&, A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, const A2&, A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, const A2&, A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, A2&, const A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, A2&, const A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<A1&, const A2&, const A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) {
    return l(type<result_type>(), f, list4<const A1&, const A2&, const A3&, const A4&>(a1, a2, a3, a4));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, const A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, const A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, const A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, const A3&, A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, const A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, const A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, const A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, const A3&, const A4&, A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, const A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, const A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, const A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, const A3&, A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, A2&, const A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, A2&, const A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<A1&, const A2&, const A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) {
    return l(type<result_type>(), f, list5<const A1&, const A2&, const A3&, const A4&, const A5&>(a1, a2, a3, a4, a5));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5, A6 &a6) {
    return l(type<result_type>(), f, list6<A1&, A2&, A3&, A4&, A5&, A6&>(a1, a2, a3, a4, a5, a6));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6) {
    return l(type<result_type>(), f, list6<const A1&, const A2&, const A3&, const A4&, const A5&, const A6&>(a1, a2, a3, a4, a5, a6));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5, A6 &a6, A7 &a7) {
    return l(type<result_type>(), f, list7<A1&, A2&, A3&, A4&, A5&, A6&, A7&>(a1, a2, a3, a4, a5, a6, a7));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7) {
    return l(type<result_type>(), f, list7<const A1&, const A2&, const A3&, const A4&, const A5&, const A6&, const A7&>(a1, a2, a3, a4, a5, a6, a7));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5, A6 &a6, A7 &a7, A8 &a8) {
    return l(type<result_type>(), f, list8<A1&, A2&, A3&, A4&, A5&, A6&, A7&, A8&>(a1, a2, a3, a4, a5, a6, a7, a8));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8) {
    return l(type<result_type>(), f, list8<const A1&, const A2&, const A3&, const A4&, const A5&, const A6&, const A7&, const A8&>(a1, a2, a3, a4, a5, a6, a7, a8));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  result_type operator()(A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5, A6 &a6, A7 &a7, A8 &a8, A9 &a9) {
    return l(type<result_type>(), f, list9<A1&, A2&, A3&, A4&, A5&, A6&, A7&, A8&, A9&>(a1, a2, a3, a4, a5, a6, a7, a8, a9));
  }

  template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  result_type operator()(const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9) {
    return l(type<result_type>(), f, list9<const A1&, const A2&, const A3&, const A4&, const A5&, const A6&, const A7&, const A8&, const A9&>(a1, a2, a3, a4, a5, a6, a7, a8, a9));
  }
