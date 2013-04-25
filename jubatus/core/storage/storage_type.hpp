// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_CORE_STORAGE_STORAGE_TYPE_HPP_
#define JUBATUS_CORE_STORAGE_STORAGE_TYPE_HPP_

#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <msgpack.hpp>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/serialization.h>
#include "bit_vector.hpp"

namespace jubatus {
namespace core {
namespace storage {

typedef pfi::data::unordered_map<uint64_t, float> row_t;
typedef pfi::data::unordered_map<std::string, row_t> tbl_t;

typedef pfi::data::unordered_map<std::string, bit_vector> bit_table_t;

typedef pfi::data::unordered_map<std::string, float> map_float_t;
typedef std::map<uint64_t, float> imap_float_t;

typedef double val1_t;

struct val2_t {
  val2_t()
      : v1(0.0),
        v2(0.0) {
  }  // undefined
  val2_t(double v1, double v2)
      : v1(v1),
        v2(v2) {
  }
  double v1;
  double v2;

  bool operator ==(const val2_t& v) const {
    return v1 == v.v1 && v2 == v.v2;
  }

  bool operator <(const val2_t& v) const {
    return (v1 != v.v1) ? v1 < v.v1 : v2 < v.v2;
  }

  val2_t operator +(const val2_t& r) const {
    val2_t ret(*this);
    ret += r;
    return ret;
  }

  val2_t& operator +=(const val2_t& r) {
    v1 += r.v1;
    v2 += r.v2;
    return *this;
  }

  val2_t operator -(const val2_t& r) const {
    val2_t ret(*this);
    ret -= r;
    return ret;
  }

  val2_t& operator -=(const val2_t& r) {
    v1 -= r.v1;
    v2 -= r.v2;
    return *this;
  }

  val2_t operator *(const val2_t& r) const {
    val2_t ret(*this);
    ret *= r;
    return ret;
  }

  val2_t& operator *=(const val2_t& r) {
    v1 *= r.v1;
    v2 *= r.v2;
    return *this;
  }

  val2_t operator /(const val2_t& r) const {
    val2_t ret(*this);
    ret /= r;
    return ret;
  }

  val2_t& operator /=(const val2_t& r) {
    v1 /= r.v1;
    v2 /= r.v2;
    return *this;
  }

  MSGPACK_DEFINE(v1, v2);

 private:
  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(v1) & MEMBER(v2);
  }
};

struct val3_t {
  val3_t()
      : v1(0.0),
        v2(0.0),
        v3(0.0) {
  }  // undefined
  val3_t(double v1, double v2, double v3)
      : v1(v1),
        v2(v2),
        v3(v3) {
  }
  double v1;
  double v2;
  double v3;

  bool operator ==(const val3_t& v) const {
    return v1 == v.v1 && v2 == v.v2 && v3 == v.v3;
  }

  bool operator <(const val3_t& v) const {
    return (v1 != v.v1) ? v1 < v.v1 : (v2 != v.v2) ? v2 < v.v2 : v3 < v.v3;
  }

  val3_t operator +(const val3_t& r) const {
    val3_t ret(*this);
    ret += r;
    return ret;
  }

  val3_t& operator +=(const val3_t& r) {
    v1 += r.v1;
    v2 += r.v2;
    v3 += r.v3;
    return *this;
  }

  val3_t operator -(const val3_t& r) const {
    val3_t ret(*this);
    ret -= r;
    return ret;
  }

  val3_t& operator -=(const val3_t& r) {
    v1 -= r.v1;
    v2 -= r.v2;
    v3 -= r.v3;
    return *this;
  }

  val3_t operator *(const val3_t& r) const {
    val3_t ret(*this);
    ret *= r;
    return ret;
  }

  // pin-point: add other operators
  val3_t operator *(double d) const {
    val3_t ret(*this);
    ret.v1 *= d;
    ret.v2 *= d;
    ret.v3 *= d;
    return ret;
  }

  val3_t& operator *=(const val3_t& r) {
    v1 *= r.v1;
    v2 *= r.v2;
    v3 *= r.v3;
    return *this;
  }

  val3_t operator /(const val3_t& r) const {
    val3_t ret(*this);
    ret /= r;
    return ret;
  }

  val3_t& operator /=(const val3_t& r) {
    v1 /= r.v1;
    v2 /= r.v2;
    v3 /= r.v3;
    return *this;
  }

  MSGPACK_DEFINE(v1, v2, v3);

 private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(v1) & MEMBER(v2) & MEMBER(v3);
  }
};

typedef std::vector<std::pair<std::string, val1_t> > feature_val1_t;
typedef std::vector<std::pair<std::string, val2_t> > feature_val2_t;
typedef std::vector<std::pair<std::string, val3_t> > feature_val3_t;

typedef std::vector<std::pair<std::string, feature_val1_t> > features1_t;
typedef std::vector<std::pair<std::string, feature_val2_t> > features2_t;
typedef std::vector<std::pair<std::string, feature_val3_t> > features3_t;

typedef pfi::data::unordered_map<std::string, val1_t> map_feature_val1_t;
typedef pfi::data::unordered_map<std::string, val3_t> map_feature_val3_t;
typedef pfi::data::unordered_map<std::string, map_feature_val3_t>
    map_features3_t;

namespace detail {

template <class E, class F>
std::vector<std::pair<std::string, E> >& binop(
    std::vector<std::pair<std::string, E> >& lhs,
    std::vector<std::pair<std::string, E> > rhs,
    F f,
    E default_value = E()) {
  std::sort(lhs.begin(), lhs.end());
  std::sort(rhs.begin(), rhs.end());

  size_t li = 0;
  size_t lsize = lhs.size();
  for (size_t ri = 0; ri < rhs.size(); ++ri) {
    while (li < lsize && lhs[li].first < rhs[ri].first) {
      lhs[li].second = f(lhs[li].second, default_value);
      ++li;
    }

    if (li < lsize && lhs[li].first == rhs[ri].first) {
      lhs[li].second = f(lhs[li].second, rhs[ri].second);
      ++li;
    } else {
      lhs.push_back(make_pair(rhs[ri].first, f(default_value, rhs[ri].second)));
    }
  }
  while (li < lsize) {
    lhs[li].second = f(lhs[li].second, default_value);
    ++li;
  }

  return lhs;
}

template <class E>
std::vector<std::pair<std::string, E> >& mult_scalar(
    std::vector<std::pair<std::string, E> >& lhs,
    double d) {
  for (size_t i = 0; i < lhs.size(); ++i) {
    lhs[i].second = lhs[i].second * d;
  }
  return lhs;
}

}  // namespace detail

template <class E>
inline std::vector<std::pair<std::string, E> > operator +(
    std::vector<std::pair<std::string, E> > lhs,
    const std::vector<std::pair<std::string, E> >& rhs) {
  return lhs += rhs;
}

template <class E>
inline std::vector<std::pair<std::string, E> >&
operator +=(std::vector<std::pair<std::string, E> >& lhs,
            const std::vector<std::pair<std::string, E> >& rhs) {
  return detail::binop(lhs, rhs, std::plus<E>());
}

template <class E>
inline std::vector<std::pair<std::string, E> > operator -(
    std::vector<std::pair<std::string, E> > lhs,
    const std::vector<std::pair<std::string, E> >& rhs) {
  return lhs -= rhs;
}

template <class E>
inline std::vector<std::pair<std::string, E> >&
operator -=(std::vector<std::pair<std::string, E> >& lhs,
            const std::vector<std::pair<std::string, E> >& rhs) {
  return detail::binop(lhs, rhs, std::minus<E>());
}

template <class E>
inline std::vector<std::pair<std::string, E> > operator *(
    std::vector<std::pair<std::string, E> > lhs,
    const std::vector<std::pair<std::string, E> >& rhs) {
  return lhs *= rhs;
}

template <class E>
inline std::vector<std::pair<std::string, E> >&
operator *=(std::vector<std::pair<std::string, E> >& lhs,
            const std::vector<std::pair<std::string, E> >& rhs) {
  return detail::binop(lhs, rhs, std::multiplies<E>());
}

template <class E>
inline std::vector<std::pair<std::string, E> > operator /(
    std::vector<std::pair<std::string, E> > lhs,
    const std::vector<std::pair<std::string, E> >& rhs) {
  return lhs /= rhs;
}

template <class E>
inline std::vector<std::pair<std::string, E> >& operator /=(
    std::vector<std::pair<std::string, E> >& lhs,
    const std::vector<std::pair<std::string, E> >& rhs) {
  return detail::binop(lhs, rhs, std::divides<E>());
}

template <class E>
inline std::vector<std::pair<std::string, E> > operator *(
    std::vector<std::pair<std::string, E> > lhs,
    double d) {
  return lhs *= d;
}

template <class E>
inline std::vector<std::pair<std::string, E> >& operator *=(
    std::vector<std::pair<std::string, E> >& lhs,
    double d) {
  return detail::mult_scalar(lhs, d);
}

template <class E>
inline std::vector<std::pair<std::string, E> > operator /(
    std::vector<std::pair<std::string, E> > lhs,
    double d) {
  return lhs /= d;
}

template <class E>
inline std::vector<std::pair<std::string, E> >& operator /=(
    std::vector<std::pair<std::string, E> >& lhs,
    double d) {
  return detail::mult_scalar(lhs, 1.0 / d);
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_STORAGE_TYPE_HPP_
