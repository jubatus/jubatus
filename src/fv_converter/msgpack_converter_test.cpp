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

#include <gtest/gtest.h>
#include <msgpack.hpp>
#include <vector>
#include <map>

#include "msgpack_converter.hpp"
#include "datum.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;

template <typename T>
void make(const T& data, datum& datum) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, data);
  
  msgpack::unpacked msg;
  msgpack::unpack(&msg, sbuf.data(), sbuf.size());
  msgpack::object obj = msg.get();
  msgpack_converter::convert(obj, datum);
}

TEST(msgpack_converter, empty) {
  datum datum;
  make(vector<string>(), datum);
  
  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(0u, datum.num_values_.size());
}

TEST(msgpack_converter, nil) {
  datum datum;
  msgpack::object nil;
  nil.type = msgpack::type::NIL;
  make(nil, datum);

  ASSERT_EQ(1u, datum.string_values_.size());
  ASSERT_EQ(0u, datum.num_values_.size());
  ASSERT_EQ("", datum.string_values_[0].first);
  ASSERT_EQ("NULL", datum.string_values_[0].second);
}

TEST(msgpack_converter, boolean) {
  datum datum;
  make(true, datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(1u, datum.num_values_.size());
  ASSERT_EQ("", datum.num_values_[0].first);
  ASSERT_EQ(1.0, datum.num_values_[0].second);
}

TEST(msgpack_converter, int64r) {
  datum datum;
  make<int64_t>(-1, datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(1u, datum.num_values_.size());
  ASSERT_EQ("", datum.num_values_[0].first);
  ASSERT_EQ(-1.0, datum.num_values_[0].second);
}

TEST(msgpack_converter, uint64r) {
  datum datum;
  make<uint64_t>(1, datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(1u, datum.num_values_.size());
  ASSERT_EQ("", datum.num_values_[0].first);
  ASSERT_EQ(1.0, datum.num_values_[0].second);
}

TEST(msgpack_converter, double) {
  datum datum;
  make<double>(1., datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(1u, datum.num_values_.size());
  ASSERT_EQ("", datum.num_values_[0].first);
  ASSERT_EQ(1.0, datum.num_values_[0].second);
}

TEST(msgpack_converter, raw) {
  datum datum;
  make<string>("hello", datum);

  ASSERT_EQ(1u, datum.string_values_.size());
  ASSERT_EQ(0u, datum.num_values_.size());
  ASSERT_EQ("", datum.string_values_[0].first);
  ASSERT_EQ("hello", datum.string_values_[0].second);
}

TEST(msgpack_converter, array) {
  datum datum;
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  make(v, datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(2u, datum.num_values_.size());

  ASSERT_EQ("[0]", datum.num_values_[0].first);
  ASSERT_EQ(1., datum.num_values_[0].second);

  ASSERT_EQ("[1]", datum.num_values_[1].first);
  ASSERT_EQ(2., datum.num_values_[1].second);
}

TEST(msgpack_converter, map) {
  {
    datum datum;
    map<string, string> m;
    m["age"] = "20";
    m["name"] = "taro";
    make(m, datum);

    ASSERT_EQ(2u, datum.string_values_.size());
    ASSERT_EQ(0u, datum.num_values_.size());

    ASSERT_EQ("/\"age\"", datum.string_values_[0].first);
    ASSERT_EQ("20", datum.string_values_[0].second);

    ASSERT_EQ("/\"name\"", datum.string_values_[1].first);
    ASSERT_EQ("taro", datum.string_values_[1].second);
  }

  {
    datum datum;
    map<string, map<string, int> > m;
    m["hanako"]["age"] = 25;
    m["taro"]["age"] = 20;
    make(m, datum);

  ASSERT_EQ(0u, datum.string_values_.size());
  ASSERT_EQ(2u, datum.num_values_.size());

  ASSERT_EQ("/\"hanako\"/\"age\"", datum.num_values_[0].first);
  ASSERT_EQ(25., datum.num_values_[0].second);

  ASSERT_EQ("/\"taro\"/\"age\"", datum.num_values_[1].first);
  ASSERT_EQ(20., datum.num_values_[1].second);
  }

}

