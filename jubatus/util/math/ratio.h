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

#ifndef INCLUDE_GUARD_PFI_MATH_RATIO_H_
#define INCLUDE_GUARD_PFI_MATH_RATIO_H_

#include <cstdlib>
#include <iostream>

namespace jubatus {
namespace util{
namespace math{
namespace ratio{

/**
   template class for rational numbers.
*/
template<class T> class ratio{
public:
  ratio():num_(0), den_(1){}
  explicit ratio(const T &a):num_(a), den_(1){}
  ratio(const T &num0, const T &den0): num_(num0), den_(den0){
    if(den_<0){
      den_*=-1; num_*=-1;
    }
    reduce();
  }

  const T& num()const{return num_;}
  const T& den()const{return den_;}
  
private:
  T num_, den_;
  static T gcd(T a, T b){
    a=abs(a); b=abs(b);
    while(true){
      a=a%b;
      if(a==0) return b;
      b=b%a;
      if(b==0) return a;
    }
  }
  static T lcm(const T &a, const T &b){
    return (a/gcd(a,b))*b;
  }
  ratio<T>& reduce(){
    /// reduce itself to its lowest terms and return *this
    T g=gcd(num_,den_)*(den_>0?1:-1);
    num_/=g; den_/=g;
    return *this;
  }

public:
  template<class T2>
  friend ratio<T2>& operator+=(ratio<T2> &a, const ratio<T2> &b);
  template<class T2>
  friend ratio<T2>& operator-=(ratio<T2> &a, const ratio<T2> &b);
  template<class T2>
  friend ratio<T2>& operator*=(ratio<T2> &a, const ratio<T2> &b);
  template<class T2>
  friend ratio<T2>& operator/=(ratio<T2> &a, const ratio<T2> &b);
};

template<class T>
inline ratio<T>& operator+=(ratio<T> &a, const ratio<T> &b){
  T gcd=ratio<T>::gcd(a.den_, b.den_);
  a.num_=a.num_*(b.den_/gcd)+b.num_*(a.den_/gcd);
  a.den_*=(b.den_/gcd);
  return a.reduce();
}

template<class T>
inline ratio<T>& operator-=(ratio<T> &a, const ratio<T> &b){
  T gcd=ratio<T>::gcd(a.den_, b.den_);
  a.num_=a.num_*(b.den_/gcd)-b.num_*(a.den_/gcd);
  a.den_*=(b.den_/gcd);
  return a.reduce();
}

template<class T>
inline ratio<T>& operator*=(ratio<T> &a, const ratio<T> &b){
  T gcd1=ratio<T>::gcd(a.num_, b.den_);
  T gcd2=ratio<T>::gcd(b.num_, a.den_);
  a.num_=(a.num_/gcd1)*(b.num_/gcd2);
  a.den_=(a.den_/gcd2)*(b.den_/gcd1);
  return a;
}

template<class T>
inline ratio<T>& operator/=(ratio<T> &a, const ratio<T> &b){
  T gcd1=ratio<T>::gcd(a.num_, b.num_);
  T gcd2=ratio<T>::gcd(a.den_, b.den_);
  a.num_=(a.num_/gcd1)*(b.den_/gcd2);
  a.den_=(a.den_/gcd2)*(b.num_/gcd1);
  if(a.den_<0){
    a.num_*=-1; a.den_*=-1;
  }
  return a;
}

template<class T>
inline const ratio<T> operator+(ratio<T> a, const ratio<T> &b){
  return a+=b;
}
template<class T>
inline const ratio<T> operator-(ratio<T> a, const ratio<T> &b){
  return a-=b;
}
template<class T>
inline const ratio<T> operator*(ratio<T> a, const ratio<T> &b){
  return a*=b;
}
template<class T>
inline const ratio<T> operator/(ratio<T> a, const ratio<T> &b){
  return a/=b;
}

template<class T>
inline bool operator==(const ratio<T> &a, const ratio<T> &b){
  return a.num()==b.num() && a.den()==b.den();
}

template <class T>
std::ostream& operator<<(std::ostream &out, 
                         const ratio<T> &r){
  out << r.num() << "/" << r.den();
  return out;
}

}
}
}
}
#endif // #ifndef INCLUDE_GUARD_PFI_MATH_RATIO_H_
