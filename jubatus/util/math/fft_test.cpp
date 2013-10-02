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

#include <gtest/gtest.h>

#include "fft.h"

#include <cmath>
#include <complex>
#include <vector>

using namespace std;
using namespace jubatus::util::math;

typedef complex<double> cplx;

template <class T>
ostream &operator<<(ostream &os, const vector<T> &v)
{
  for (size_t i=0; i<v.size(); i++){
    if (i>0) os<<", ";
    os<<v[i];
  }
  return os;
}

TEST(fft, length_test)
{
  vector<cplx> dat;
  EXPECT_THROW(fft(dat.begin(), dat.end()), length_error);
  dat.resize(3);
  EXPECT_THROW(fft(dat.begin(), dat.end()), length_error);
  dat.resize(127);
  EXPECT_THROW(fft(dat.begin(), dat.end()), length_error);
  dat.resize(1);
  EXPECT_NO_THROW(fft(dat.begin(), dat.end()));
  dat.resize(8);
  EXPECT_NO_THROW(fft(dat.begin(), dat.end()));
  dat.resize(256);
  EXPECT_NO_THROW(fft(dat.begin(), dat.end()));
}

double frand(double low, double high)
{
  double r=(double)rand()/RAND_MAX;
  return (high-low)*r+low;
}

inline bool double_eq(double a, double b)
{
  double err=abs(a-b);
  return (err/abs(a))<1e-8;
}

#define ASSERT_COMPLEX_VECTOR_EQ(a, b)			\
  ASSERT_EQ(a.size(), b.size());			\
  for (size_t i=0; i<a.size(); i++){			\
    ASSERT_PRED2(double_eq, a[i].real(), b[i].real());	\
    ASSERT_PRED2(double_eq, a[i].imag(), b[i].imag());	\
  }

TEST(fft, fft_ifft)
{
  for (int i=0;i<100;i++){
    int n=1<<(rand()%16+1);
    vector<cplx> dat(n);
    for (int i=0;i<n;i++)
      dat[i]=cplx(frand(-1e5, 1e5), frand(-1e5, 1e5));
    vector<cplx> to(dat);
    fft(to.begin(), to.end());
    ifft(to.begin(), to.end());
    for (int i=0;i<n;i++)
      to[i]/=n;
    ASSERT_COMPLEX_VECTOR_EQ(dat, to);
  }
}

TEST(fft, fft)
{
  double dat[][16]={
    {68037.543430941907,-21123.414636181391, },
    {68037.543430941907,-21123.414636181391, },
    {56619.844751721161,59688.006695214659, 82329.471587356849,-60489.726141323205, },
    {138949.316339078010,-801.719446108546, -25709.626835635689,120177.732836537863, },
    {-32955.448857022202,53645.918962380791, -44445.057839362446,10793.991159086101, -4520.589627567955,25774.184952384880, -27043.105441631327,2680.182039123101, },
    {-108964.201765583930,92894.277112974873, -20321.050109491247,45273.686407727029, 34012.124796403616,65945.930716556468, -36548.668349417247,10469.781612264793, },
    {90445.945034942561,83239.013600740116, 27142.345591980193,43459.385886536620, -71679.488928839331,21393.775251411731, -96739.885675134079,-51422.645874052607, -72553.684642796259,60835.350845398090, -68664.182149183092,-19811.121150763298, -74041.910643708849,-78238.239594846149, 99784.903600711783,-56348.618937818625, },
    {-166305.957812027074,3106.900026605887, 517563.294672435382,132515.622378821485, 295033.189512339013,245485.683272353257, -95119.648251923427,54728.834572851090, -89352.320548776668,171352.900178801676, 7699.994375557959,-92433.140297876482, 32194.130417049921,156351.974306792021, 221854.877914885292,-5196.665632428008, },
  };

  for (int i=0;i<4;i++){
    int n=1<<i;
    vector<cplx> v(n), w(n);

    for (int j=0; j<n; j++) v[j]=cplx(dat[i*2+0][j*2+0], dat[i*2+0][j*2+1]);
    for (int j=0; j<n; j++) w[j]=cplx(dat[i*2+1][j*2+0], dat[i*2+1][j*2+1]);

    ifft(v.begin(), v.end());
    ASSERT_COMPLEX_VECTOR_EQ(v, w);
  }
}

TEST(fft, fft2d)
{
  int const m = 32, n = 64;
  double const pi = std::acos(-1);
  std::complex<double> a[m][n];
  for (int i = 0; i < m; ++i)
    for (int j = 0; j < n; ++j)
      a[i][j] = std::cos(2*pi * i / m) * std::cos(2*pi * j / n);

  jubatus::util::math::fft2d(&a[0][0], &a[m][0], n);

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if ((i == 1 || i == m-1) && (j == 1 || j == n-1))
        EXPECT_NEAR(m*n / 4, a[i][j].real(), 1e-5);
      else
        EXPECT_NEAR(0, a[i][j].real(), 1e-5);
      EXPECT_NEAR(0, a[i][j].imag(), 1e-5);
    }
  }
}

TEST(fft, fft2d_matrix)
{
  int const m = 32, n = 64;
  double const pi = std::acos(-1);
  std::vector<std::vector<std::complex<double> > > a(m, std::vector<std::complex<double> >(n));
  for (int i = 0; i < m; ++i)
    for (int j = 0; j < n; ++j)
      a[i][j] = std::cos(2*pi * i / m) * std::cos(2*pi * j / n);

  jubatus::util::math::fft2d(a);

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if ((i == 1 || i == m-1) && (j == 1 || j == n-1))
        EXPECT_NEAR(m*n / 4, a[i][j].real(), 1e-5);
      else
        EXPECT_NEAR(0, a[i][j].real(), 1e-5);
      EXPECT_NEAR(0, a[i][j].imag(), 1e-5);
    }
  }
}
