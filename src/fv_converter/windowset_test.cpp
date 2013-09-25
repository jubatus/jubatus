// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/shared_ptr.h>
#include "windowset.hpp"
#include "key_matcher.hpp"
#include "match_all.hpp"
#include "windowset_config.hpp"

namespace jubatus {
namespace fv_converter {

TEST(windowset, construct) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  windowset<int> a(confv);
  windowset<double> b(confv);
  windowset<std::string> c(confv);
  windowset<std::vector<int> > d(confv);
  windowset<int> e(confv);
}

TEST(windowset, push) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  confv->push_back(windowset_config(
      pfi::lang::shared_ptr<key_matcher>(new match_all()),
      10,
      window::DATUM_UNIT));
  windowset<int> a(confv);  // length 10
  for (int i = 0; i < 5; ++i) {
    windowset<int>::one_datum_t one;
    one.push_back(std::make_pair(0, i));
    windowset<int>::datum_t d;
    d.push_back(std::make_pair("a", one));
    a.push(d);
  }
}

TEST(windowset, windowize) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  confv->push_back(windowset_config(
      pfi::lang::shared_ptr<key_matcher>(new match_all()),
      10,
      window::DATUM_UNIT));
  windowset<int> a(confv);  // length 10
  for (int i = 0; i < 5; ++i) {
    windowset<int>::one_datum_t one;
    one.push_back(std::make_pair(0, i));
    windowset<int>::datum_t d;
    d.push_back(std::make_pair("a", one));
    a.push(d);
  }

  sfv_t fv;
  windowset<int>::datum_t kvp;
  a.windowize(kvp, fv);

  ASSERT_EQ(kvp.front().first, "a");
  std::vector<std::pair<uint64_t, int> >& result = kvp.front().second;
  ASSERT_EQ(5u, result.size());
  {
    for (int i = 0; i < 5; ++i) {
      ASSERT_EQ(i, result[i].second);
    }
  }
}

TEST(windowset, push_many) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  confv->push_back(windowset_config(
      pfi::lang::shared_ptr<key_matcher>(new match_all()),
      10,
      window::DATUM_UNIT));
  windowset<int> a(confv);  // length 10
  for (int i = 0; i < 20; ++i) {
    windowset<int>::one_datum_t one;
    one.push_back(std::make_pair(0, i));
    windowset<int>::datum_t d;
    d.push_back(std::make_pair("a", one));
    a.push(d);
  }

  sfv_t fv;
  windowset<int>::datum_t kvp;
  a.windowize(kvp, fv);

  std::vector<std::pair<uint64_t, int> >& result = kvp.front().second;
  ASSERT_EQ(9u, result.size());
  {
    for (int i = 11; i < 20; ++i) {
      ASSERT_EQ(i, result[i-11].second);
    }
  }
}

TEST(windowset, push_many2) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  confv->push_back(windowset_config(
      pfi::lang::shared_ptr<key_matcher>(new match_all()),
      100,
      window::DATUM_UNIT));
  windowset<int> a(confv);  // length 100
  for (int i = 0; i < 2000; ++i) {
    windowset<int>::one_datum_t one;
    one.push_back(std::make_pair(0, i));
    windowset<int>::datum_t d;
    d.push_back(std::make_pair("a", one));
    a.push(d);
  }

  sfv_t fv;
  for (int i = 0; i < 10; ++i) {
    windowset<int>::datum_t kvp;
    a.windowize(kvp, fv);
    windowset<int>::one_datum_t& result = kvp.front().second;
    ASSERT_EQ(99u, result.size());
    {
      for (int i = 1901; i < 2000; ++i) {
        EXPECT_EQ(i, result[i-1901].second);
      }
    }
  }
}

TEST(windowset, many_window) {
  pfi::lang::shared_ptr<std::vector<windowset_config> >
      confv(new std::vector<windowset_config>());
  confv->push_back(windowset_config(
      pfi::lang::shared_ptr<key_matcher>(new match_all()),
      5,
      window::DATUM_UNIT));
  windowset<size_t> a(confv);  // length 100

  for (size_t j = 0; j < 5; ++j) {
    for (size_t i = 0; i < 30; ++i) {
      windowset<size_t>::one_datum_t one;
      one.push_back(std::make_pair(0, i*(j+1)));
      windowset<size_t>::datum_t d;
      d.push_back(std::make_pair(pfi::lang::lexical_cast<std::string>(j), one));
      a.push(d);
    }

    sfv_t fv;
    windowset<size_t>::datum_t kvp;
    a.windowize(kvp, fv);

    ASSERT_EQ(j+1, kvp.size());  // datum should contain old datum windows
    for (size_t i = 0; i < kvp.size(); ++i) {
      for (size_t k = 0; k < kvp[i].second.size(); ++k) {
        if (kvp[i].first == pfi::lang::lexical_cast<std::string>(j)) {
          ASSERT_EQ((k+26)*(j+1), kvp[i].second[k].second);
        }
        /*
        std::cout << kvp[i].second[k].first << ":" << kvp[i].second[k].second;
        if (k+1 < kvp[i].second.size()){
          std::cout << ", ";
        }
        */
      }
    }
  }
}

}  // namespace fv_converter
}  // jubatus
