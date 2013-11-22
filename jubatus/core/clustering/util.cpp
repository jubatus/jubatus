// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "util.hpp"

#include <cfloat>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace clustering {

void concat(const wplist& src, wplist& dst) {
  dst.insert(dst.end(), src.begin(), src.end());
}

char digit(int num, int r, int n) {
  if (r < 0) {
    return 0;
  }
  for (int i = 0; i < r; ++i) {
    num /= n;
  }
  return num % n;
}

double sum(const common::sfv_t& p) {
  double s = 0;
  for (common::sfv_t::const_iterator it = p.begin(); it != p.end(); ++it) {
    s += (*it).second;
  }
  return s;
}
double sum2(const common::sfv_t& p) {
  double s = 0;
  for (common::sfv_t::const_iterator it = p.begin(); it != p.end(); ++it) {
    s += std::pow((*it).second, 2);
  }
  return s;
}

void scalar_mul_and_add(
    const common::sfv_t& left,
    float s, common::sfv_t& right) {
  common::sfv_t::const_iterator l = left.begin();
  common::sfv_t::iterator       r = right.begin();
  while (l != left.end() && r != right.end()) {
    if (l->first < r->first) {
      std::pair<std::string, float> p = *l;
      p.second *= s;
      r = right.insert(r, p);
      ++l;
    } else if (l->first > r->first) {
      ++r;
    } else {
      r->second += l->second * s;
      ++l;
      ++r;
    }
  }
  for (; l != left.end(); ++l) {
    std::pair<std::string, float> p = *l;
    p.second *= s;
    right.push_back(p);
  }
}

common::sfv_t add(const common::sfv_t& p1, const common::sfv_t& p2) {
  common::sfv_t ret;
  common::sfv_t::const_iterator it1 = p1.begin();
  common::sfv_t::const_iterator it2 = p2.begin();
  while (it1 != p1.end() && it2 != p2.end()) {
    if ((*it1).first < (*it2).first) {
      ret.push_back((*it1));
      ++it1;
    } else if ((*it1).first > (*it2).first) {
      ret.push_back((*it2));
      ++it2;
    } else {
      ret.push_back(make_pair((*it1).first, (*it1).second + (*it2).second));
      ++it1;
      ++it2;
    }
  }
  for (; it1 != p1.end(); ++it1) {
    ret.push_back((*it1));
  }
  for (; it2 != p2.end(); ++it2) {
    ret.push_back((*it2));
  }

  return ret;
}

common::sfv_t sub(const common::sfv_t& p1, const common::sfv_t& p2) {
  common::sfv_t ret;
  common::sfv_t::const_iterator it1 = p1.begin();
  common::sfv_t::const_iterator it2 = p2.begin();
  while (it1 != p1.end() && it2 != p2.end()) {
    if ((*it1).first < (*it2).first) {
      ret.push_back((*it1));
      ++it1;
    } else if ((*it1).first > (*it2).first) {
      ret.push_back(make_pair((*it2).first, -(*it2).second));
      ++it2;
    } else {
      ret.push_back(make_pair((*it1).first, (*it1).second - (*it2).second));
      ++it1;
      ++it2;
    }
  }
  for (; it1 != p1.end(); ++it1) {
    ret.push_back((*it1));
  }
  for (; it2 != p2.end(); ++it2) {
    ret.push_back(make_pair((*it2).first, -(*it2).second));
  }

  return ret;
}

common::sfv_t scalar_dot(const common::sfv_t& p, double s) {
  common::sfv_t ret;
  for (common::sfv_t::const_iterator it = p.begin(); it != p.end(); ++it)  {
    ret.push_back(make_pair((*it).first, (*it).second*s));
  }
  return ret;
}

double dist(const common::sfv_t& p1, const common::sfv_t& p2) {
  double ret = 0;
  common::sfv_t::const_iterator it1 = p1.begin();
  common::sfv_t::const_iterator it2 = p2.begin();
  while (it1 != p1.end() && it2 != p2.end()) {
    int cmp = strcmp(it1->first.c_str(), it2->first.c_str());
    if (cmp < 0) {
      ret += it1->second * it1->second;
      ++it1;
    } else if (cmp > 0) {
      ret += it2->second * it2->second;
      ++it2;
    } else {
      ret += (it1->second  - it2->second) * (it1->second - it2->second);
      ++it1;
      ++it2;
    }
  }
  for (; it1 != p1.end(); ++it1) {
    ret += pow(it1->second, 2);
  }
  for (; it2 != p2.end(); ++it2) {
    ret += pow(it2->second, 2);
  }
  return sqrt(ret);
}

double dist(const weighted_point &d1, const weighted_point &d2) {
  return dist(d1.data, d2.data);
}

pair<size_t, double> min_dist(
    const common::sfv_t& p, const vector<common::sfv_t>& P) {
  size_t idx = 0;
  double mindist = DBL_MAX;
  for (vector<common::sfv_t>::const_iterator it = P.begin();
       it != P.end();
       ++it) {
    double d = dist(p, *it);
    if (mindist > d) {
      idx = it - P.begin();
      mindist = d;
    }
  }
  return std::make_pair(idx, mindist);
}

std::pair<size_t, double> min_dist(const weighted_point& d1, const wplist& P) {
  double md = DBL_MAX;
  csize_t midx = 0;
  for (wplist::const_iterator it = P.begin(); it != P.end(); ++it) {
    double d = dist((*it), d1);
    if (md > d) {
      midx = it - P.begin();
      md = d;
    }
  }
  return std::make_pair(midx, md);
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
