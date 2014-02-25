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

#ifndef JUBATUS_UTIL_MATH_FFT_H_
#define JUBATUS_UTIL_MATH_FFT_H_

#include <iterator>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <unistd.h>

namespace jubatus {
namespace util{
namespace math{

namespace detail{

template <class Iterator, class Complex>
inline void fft_inner(Iterator a, Iterator b, int flip)
{
  typedef typename Complex::value_type value_type;

  ssize_t n=b-a;

  value_type pi=std::atan(1.0)*4;
  value_type theta=2.0*pi/n*flip;

  for (ssize_t i=0, j=1; j<n-1; j++){
    for (ssize_t k=n>>1; k>(i^=k); k>>=1);
    if (j<i) swap(a[j], a[i]);
  }

  for (ssize_t m=0, mh=1; (m=(mh<<1))<=n; mh=m){
    int irev=0;
    for (ssize_t i=0; i<n; i+=m){
      Complex w(std::cos(theta*irev), std::sin(theta*irev));
      for (ssize_t k=n>>2; k>(irev^=k); k>>=1);
      for (ssize_t j=i; j<mh+i; j++){
        ssize_t k=j+mh;
        Complex x=a[j]-a[k];
        a[j]+=a[k];
        //a[k]=w*x;
        a[k] = Complex(w.real()*x.real()-w.imag()*x.imag(),
                       w.real()*x.imag()+w.imag()*x.real());
      }
    }
  }
}

inline bool is_pow2(ssize_t s)
{
  if (s<=0) return false;
  return (s&(s-1))==0;
}

template <class Iterator>
inline void fft_wrap(Iterator b, Iterator e, int flip)
{
  ssize_t length=e-b;
  if (!is_pow2(length)) throw std::length_error("fft: length must be power of 2");
  fft_inner<Iterator, typename std::iterator_traits<Iterator>::value_type>(b, e, flip);
}

template <class Iterator, class Jterator>
inline void transpose(Iterator a, Iterator b, Jterator c, size_t pitch)
{
  size_t n=b-a;
  size_t h=n/pitch;
  size_t w=pitch;

  for (size_t y=0; y<h; y++)
    for (size_t x=0; x<w; x++)
      c[x*h+y]=a[y*w+x];
}

template <class Matrix, class Natrix>
inline void transpose(Matrix &a, Natrix &c)
{
  size_t h=a.size();
  size_t w=a[0].size();

  for (size_t y=0; y<h; y++)
    for (size_t x=0; x<w; x++)
      c[x][y]=a[y][x];
}

template <class Iterator>
inline void fft2d_wrap(Iterator a, Iterator b, size_t pitch, int flip)
{
  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  size_t n=b-a;
  size_t h=n/pitch;
  size_t w=pitch;
  if (h*w!=n) throw std::length_error("fft2d: argument is not rectangle");

  for (size_t y=0; y<h; y++)
    fft_wrap(a+y*w, a+y*w+w, flip);

  std::vector<value_type> t(w*h);

  transpose(a, b, t.begin(), w);

  for (size_t x=0; x<w; x++)
    fft_wrap(t.begin() + x*h, t.begin() + (x+1)*h, flip);

  transpose(t.begin(), t.end(), a, h);
}

template <class Matrix>
inline void fft2d_wrap(Matrix &a, int flip)
{
  typedef typename Matrix::value_type::value_type value_type;

  size_t h=a.size();
  if (!is_pow2(h)) throw std::length_error("fft: length must be power of 2");
  size_t w=a[0].size();
  if (!is_pow2(h)) throw std::length_error("fft: length must be power of 2");

  for (size_t y=0; y<h; y++)
    if (a[y].size()!=w)
      throw std::length_error("fft2d: argument is not rectangle");

  for (size_t y=0; y<h; y++)
    fft_wrap(a[y].begin(), a[y].end(), flip);

  std::vector<std::vector<value_type> > t(w, std::vector<value_type>(h));

  transpose(a, t);

  for (size_t x=0; x<w; x++)
    fft_wrap(t[x].begin(), t[x].end(), flip);

  transpose(t, a);
}

} // detail

template <class Iterator>
void fft(Iterator b, Iterator e)
{
  detail::fft_wrap(b, e, 1);
}

template <class Iterator>
void ifft(Iterator b, Iterator e)
{
  detail::fft_wrap(b, e, -1);
}

template <class Iterator>
void fft2d(Iterator b, Iterator e, size_t pitch)
{
  detail::fft2d_wrap(b, e, pitch, 1);
}

template <class Iterator>
void ifft2d(Iterator b, Iterator e, size_t pitch)
{
  detail::fft2d_wrap(b, e, pitch, -1);
}

template <class Matrix>
void fft2d(Matrix &a)
{
  detail::fft2d_wrap(a, 1);
}

template <class Matrix>
void ifft2d(Matrix &a)
{
  detail::fft2d_wrap(a, -1);
}

} // math
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_MATH_FFT_H_
