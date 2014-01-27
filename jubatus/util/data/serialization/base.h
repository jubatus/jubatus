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

#ifndef JUBATUS_UTIL_DATA_SERIALIZATION_BASE_H_
#define JUBATUS_UTIL_DATA_SERIALIZATION_BASE_H_

#include <cstdio>
#include <iostream>

#include "../../lang/safe_bool.h"
#include "../../system/endian_util.h"

namespace jubatus {
namespace util {
namespace data {
namespace serialization {

class access {
public:
  template<class Archive, class T>
  static void serialize(Archive& ar, T& v) {
    v.serialize(ar);
  }
};

template <class Archive, class T>
void serialize(Archive& ar, T& v)
{
  access::serialize(ar, v);
}

template <class Archive, class T>
Archive& operator&(Archive& ar, T& v)
{
  serialize(ar, v);
  return ar;
}

template <class Archive, class T>
Archive& operator&(Archive& ar, const T& v)
{
  serialize(ar, v);
  return ar;
}

class binary_iarchive : public jubatus::util::lang::safe_bool<binary_iarchive> {
  binary_iarchive(const binary_iarchive&);
  binary_iarchive& operator=(const binary_iarchive&);

public:
  binary_iarchive(std::istream& is)
    : is(is)
  {}

  static const bool is_read = true;

  template <int N>
  binary_iarchive& read(char* p) {
    return read(p, N);
  }

  binary_iarchive& read(char* p, int size) {
    is.read(p, size);
    return *this;
  }

  bool bool_test() const {
    return !is.fail();
  }

private:
  std::istream& is;
};

template <class T>
binary_iarchive& operator>>(binary_iarchive& ar, T& v)
{
  ar & v;
  return ar;
}

template <class T>
binary_iarchive& operator>>(binary_iarchive& ar, const T& v)
{
  ar & v;
  return ar;
}

#define gen_serial_binary_iarchive(tt) \
  inline void serialize(binary_iarchive& ar, tt& n) \
  { \
    tt tmp; \
    ar.read<sizeof(tmp)>(reinterpret_cast<char*>(&tmp)); \
    if (ar) n = jubatus::util::system::endian::from_little(tmp); \
  }

gen_serial_binary_iarchive(bool);
gen_serial_binary_iarchive(char);
gen_serial_binary_iarchive(signed char);
gen_serial_binary_iarchive(unsigned char);
gen_serial_binary_iarchive(short);
gen_serial_binary_iarchive(unsigned short);
gen_serial_binary_iarchive(int);
gen_serial_binary_iarchive(unsigned int);
gen_serial_binary_iarchive(long);
gen_serial_binary_iarchive(unsigned long);
gen_serial_binary_iarchive(long long);
gen_serial_binary_iarchive(unsigned long long);
gen_serial_binary_iarchive(float);
gen_serial_binary_iarchive(double);
gen_serial_binary_iarchive(long double);

#undef gen_serial_binary_iarchive

class binary_oarchive : public jubatus::util::lang::safe_bool<binary_oarchive> {
  binary_oarchive(const binary_oarchive&);
  binary_oarchive& operator=(const binary_oarchive&);

public:
  binary_oarchive(std::ostream& os)
    : os(os)
  {}
  virtual ~binary_oarchive() {}

  static const bool is_read = false;

  template <int N>
  binary_oarchive& write(const char* p) {
    return write(p, N);
  }

  binary_oarchive& write(const char* p, int size) {
    os.write(p, size);
    return *this;
  }

  void flush() {
    os.flush();
  }

  bool bool_test() const {
    return !os.fail();
  }

private:
  std::ostream& os;
};

template <class T>
binary_oarchive& operator<<(binary_oarchive& ar, T& v)
{
  ar & v;
  return ar;
}

template <class T>
binary_oarchive& operator<<(binary_oarchive& ar, const T& v)
{
  ar & v;
  return ar;
}

#define gen_serial_binary_oarchive(tt) \
    inline void serialize(binary_oarchive& ar, tt n) \
  { \
    n = jubatus::util::system::endian::to_little(n); \
    ar.write<sizeof(n)>(reinterpret_cast<const char*>(&n)); \
  }

gen_serial_binary_oarchive(bool);
gen_serial_binary_oarchive(char);
gen_serial_binary_oarchive(signed char);
gen_serial_binary_oarchive(unsigned char);
gen_serial_binary_oarchive(short);
gen_serial_binary_oarchive(unsigned short);
gen_serial_binary_oarchive(int);
gen_serial_binary_oarchive(unsigned int);
gen_serial_binary_oarchive(long);
gen_serial_binary_oarchive(unsigned long);
gen_serial_binary_oarchive(long long);
gen_serial_binary_oarchive(unsigned long long);
gen_serial_binary_oarchive(float);
gen_serial_binary_oarchive(double);
gen_serial_binary_oarchive(long double);

#undef gen_serial_binary_oarchive

} // serialization
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SERIALIZATION_BASE_H_
