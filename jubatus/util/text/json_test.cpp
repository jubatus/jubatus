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
#include <stdint.h>

#include "json.h"

#include <iostream>
#include <cmath>
#include <climits>

#include "../lang/bind.h"
#include "../lang/cast.h"
#include "../data/optional.h"

using namespace std;
using namespace jubatus::util::lang;
using namespace jubatus::util::text::json;
using namespace jubatus::util::data::serialization;

struct example1{
  struct img{
    int Width;
    int Height;
    string Title;

    struct tn{
      string Url;
      int Height;
      string Width;
      
      template <class Archive>
      void serialize(Archive &ar){
        ar & JUBA_MEMBER(Url) & JUBA_MEMBER(Height) & JUBA_MEMBER(Width);
      }

      bool operator==(const tn &r){
        return
          Url==r.Url &&
          Height==r.Height &&
          Width==r.Width;
      }

    } Thumbnail;

    vector<int> IDs;

    template <class Archive>
      void serialize(Archive &ar){
        ar & JUBA_MEMBER(Width) & JUBA_MEMBER(Height) & JUBA_MEMBER(Title) 
          & JUBA_MEMBER(Thumbnail) & JUBA_MEMBER(IDs);
      }

    bool operator==(const img &r){
      return
        Width==r.Width &&
        Height==r.Height &&
        Title==r.Title &&
        Thumbnail==r.Thumbnail &&
        IDs==r.IDs;
    }
  } Image;

  template <class Archive>
    void serialize(Archive &ar){
      ar & JUBA_MEMBER(Image);
    }

  bool operator==(const example1 &r){
    return Image==r.Image;
  }  
};

struct example2{
  string precision;
  double Latitude;
  double Longitude;
  string Address;
  string City;
  string State;
  string Zip;
  string Country;

  template <class Archive>
  void serialize(Archive &ar){
    ar
      & JUBA_MEMBER(precision)
      & JUBA_MEMBER(Latitude)
      & JUBA_MEMBER(Longitude)
      & JUBA_MEMBER(Address)
      & JUBA_MEMBER(City)
      & JUBA_MEMBER(State)
      & JUBA_MEMBER(Zip)
      & JUBA_MEMBER(Country);
  }

  bool operator==(const example2 &r) const{
    return
      precision==r.precision &&
      std::abs(Latitude-r.Latitude)<1e-10 &&
      std::abs(Longitude-r.Longitude)<1e-10 &&
      Address==r.Address &&
      City==r.City &&
      State==r.State &&
      Zip==r.Zip &&
      Country==r.Country;
  }
};

struct example3{
  example3(): a(1), b(3.14), c("hoge"), d(true), e(3.14) {}

  int a;
  double b;
  string c;
  bool d;
  float e;

  template <class Archive>
  void serialize(Archive &ar){
    ar
      & JUBA_MEMBER(a)
      & JUBA_MEMBER(b)
      & JUBA_MEMBER(c)
      & JUBA_MEMBER(d)
      & JUBA_MEMBER(e);
  }

  bool operator==(const example3 &r) const{
    return
      a==r.a &&
      b==r.b &&
      c==r.c &&
      d==r.d &&
      e==r.e;
  }
};

string to_string(const json &j)
{
  ostringstream oss;
  oss<<j;
  return oss.str();
}

TEST(json, to_json)
{
  {
    int n=123;
    ostringstream oss;
    oss<<to_json(n);
    EXPECT_EQ("123", oss.str());
  }
  {
    unsigned int n=123;
    ostringstream oss;
    oss<<to_json(n);
    EXPECT_EQ("123", oss.str());
  }
  {
    float f=3.14f;
    ostringstream oss;
    oss<<to_json(f);
    EXPECT_FLOAT_EQ(3.14f, lexical_cast<float>(oss.str()));
  }
  {
    double d=3.14;
    ostringstream oss;
    oss<<to_json(d);
    EXPECT_DOUBLE_EQ(3.14, lexical_cast<double>(oss.str()));
  }
  {
    string s="hello, sofmap world!";
    ostringstream oss;
    oss<<to_json(s);
    EXPECT_EQ("\"hello, sofmap world!\"", oss.str());
  }
  {
    bool b=true;
    ostringstream oss;
    oss<<to_json(b);
    EXPECT_EQ("true", oss.str());
  }
  {
    bool b=false;
    ostringstream oss;
    oss<<to_json(b);
    EXPECT_EQ("false", oss.str());
  }

  {
    vector<int> v;
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);

    ostringstream oss;
    oss<<to_json(v);
    EXPECT_EQ("[0,1,2]", oss.str());
  }

  struct compare_helper {
    static bool invoke(const string& s) {
      return s == "{\"abc\":1.23,\"hoge\":3.14}" ||
          s == "{\"hoge\":3.14,\"abc\":1.23}";
    }
  };
  {
    map<string, double> mm;
    
    mm["abc"]=1.23;
    mm["hoge"]=3.14;

    ostringstream oss;
    oss<<to_json(mm);
    
    EXPECT_PRED1(&compare_helper::invoke, oss.str());
  }

  {
    jubatus::util::data::unordered_map<string, double> mm;

    mm["abc"]=1.23;
    mm["hoge"]=3.14;

    ostringstream oss;
    oss<<to_json(mm);

    EXPECT_PRED1(&compare_helper::invoke, oss.str());
  }
}

TEST(json, from_json)
{
  {
    json j(new json_integer(123));
    EXPECT_EQ(123, json_cast<int>(j));
    EXPECT_EQ(123, json_cast<long int>(j));
    EXPECT_EQ(123, json_cast<long long int>(j));
  }
  {
    json j(new json_float(3.14));
    EXPECT_FLOAT_EQ(3.14f, json_cast<float>(j));
    EXPECT_DOUBLE_EQ(3.14, json_cast<double>(j));
  }
  {
    json j(new json_string("hell, world!"));
    EXPECT_EQ("hell, world!", json_cast<string>(j));
  }
  {
    json j(new json_bool(true));
    EXPECT_EQ(true, json_cast<bool>(j));
  }
  {
    json j(new json_bool(false));
    bool const expected = false;
    bool const actual = json_cast<bool>(j);
    EXPECT_EQ(expected, actual);
  }

  {
    json j(new json_array());
    j.add(new json_integer(0));
    j.add(new json_integer(1));
    j.add(new json_integer(2));

    vector<int> v;
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);

    EXPECT_EQ(true, v==json_cast<vector<int> >(j));
  }

  {
    json j(new json_array());
    j.add(new json_string("abc"));
    j.add(new json_string("def"));

    vector<string> v;
    v.push_back("abc");
    v.push_back("def");

    EXPECT_EQ(v, json_cast<vector<string> >(j));
  }

  {
    json j(new json_object);

    EXPECT_THROW(json_cast<vector<string> >(j);, json_bad_cast<vector<string> >);
  }

  {
    json j(new json_object());
    j["abc"]=json(new json_float(1.23));
    j["hoge"]=json(new json_float(3.14));

    map<string, double> mm;
    
    mm["abc"]=1.23;
    mm["hoge"]=3.14;

    EXPECT_EQ(true, (mm==json_cast<map<string, double> >(j)));
  }

  {
    json j(new json_object());
    j["abc"]=json(new json_float(1.23));
    j["hoge"]=json(new json_float(3.14));

    map<string, float> mm;
    
    mm["abc"]=1.23f;
    mm["hoge"]=3.14f;

    EXPECT_EQ(true, (mm==json_cast<map<string, float> >(j)));
  }
}

TEST(json, merge)
{
  {
    json js1(new json_object());
    js1["a"] = new json_integer(42);
    js1["c"] = new json_string("string");

    json js2(new json_object());
    js2["b"] = new json_bool(true);
    js2["c"] = new json_string("new string");

    json js3 = js1.merge(js2);
    EXPECT_EQ(42, json_cast<int>(js3["a"]));
    EXPECT_EQ(true, json_cast<bool>(js3["b"]));
    EXPECT_EQ(std::string("new string"), json_cast<std::string>(js3["c"]));

    json js4 = js1.merge(js1);
    EXPECT_EQ(42, json_cast<int>(js4["a"]));
    EXPECT_EQ(0, js4.count("b"));
    EXPECT_EQ(std::string("string"), json_cast<std::string>(js4["c"]));
  }

  {
    json ji(new json_integer(42));
    json jf(new json_float(3.14));
    json js(new json_string("string"));
    json jb(new json_bool(true));
    json ja(new json_array()); ja.add(new json_integer(1));
    json jn(new json_null());
    json jo(new json_object()); jo.add("key", new json_string("value"));

    EXPECT_THROW(ji.merge(jo), json_bad_cast<json>);
    EXPECT_THROW(jf.merge(jo), json_bad_cast<json>);
    EXPECT_THROW(js.merge(jo), json_bad_cast<json>);
    EXPECT_THROW(jb.merge(jo), json_bad_cast<json>);
    EXPECT_THROW(ja.merge(jo), json_bad_cast<json>);
    EXPECT_THROW(jn.merge(jo), json_bad_cast<json>);

    EXPECT_THROW(jo.merge(ji), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(jf), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(js), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(jb), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(ja), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(jn), json_bad_cast<json>);
    EXPECT_THROW(jo.merge(jn), json_bad_cast<json>);

    EXPECT_NO_THROW(jo.merge(jo));
  }

  {
    json js1(new json_object());
    js1["a"] = new json_array();
    js1["a"].add(new json_integer(1));
    js1["a"].add(new json_integer(2));
    js1["a"].add(new json_integer(3));
    js1["b"] = new json_object();
    js1["b"]["p"] = new json_integer(42);
    js1["b"]["q"] = new json_string("string");
    js1["b"]["r"] = new json_float(3.14);

    json js2(new json_object());
    js2["b"] = new json_object();
    js2["b"]["x"] = new json_string("another string");
    js2["b"]["y"] = new json_float(1.0);
    js2["b"]["z"] = new json_bool(false);
    js2["c"] = new json_array();
    js2["c"].add(new json_string("one"));
    js2["c"].add(new json_string("two"));
    js2["c"].add(new json_string("three"));

    json js3 = js1.merge(js2);

    js1["a"].add(new json_integer(4));
    js1["b"]["s"] = new json_float(1.414);
    js2["b"]["w"] = new json_float(1.732);
    js2["c"].add(new json_string("four"));

    EXPECT_EQ(4, json_cast<int>(js3["a"][3]));
    EXPECT_TRUE(is<json_null>(js3["b"]["s"]));
    EXPECT_EQ(1.732, json_cast<double>(js3["b"]["w"]));
    EXPECT_EQ("four", json_cast<std::string>(js3["c"][3]));
  }
}

typedef json(*merge_func_type)(json,json);
typedef json(*merge_with1_func_type)(json,json,merge_func_type);

static inline json add_json_integer(json js1, json js2)
{
  int i1 = json_cast<int>(js1);
  int i2 = json_cast<int>(js2);
  return json(new json_integer(i1+i2));
}

template <typename F>
static inline json recursive_merge_with(json js1, json js2, F f)
{
  if (is<json_object>(js1) && is<json_object>(js2)) {
    // object vs object, recursive call.
    return merge_with(js1, js2,
                      bind((merge_with1_func_type)(&recursive_merge_with), _1, _2, (merge_func_type)(f)));
  } else {
    return f(js1,js2);
  }
}

TEST(json, merge_with)
{
  {
    json js1(new json_object());
    js1["a"] = new json_integer(1);
    js1["b"] = new json_integer(2);
    json js2(new json_object());
    js2["b"] = new json_integer(20);
    js2["c"] = new json_integer(30);
    json js3 = js1.merge_with(js2,&add_json_integer);
    EXPECT_EQ( 1,json_cast<int>(js3["a"]));
    EXPECT_EQ(22,json_cast<int>(js3["b"]));
    EXPECT_EQ(30,json_cast<int>(js3["c"]));
  }
  {
    json js1(new json_object());
    js1["v"] = new json_integer(1);
    js1["n"] = new json_object();
    js1["n"]["v"] = new json_integer(2);
    js1["n"]["n"] = new json_object();
    js1["n"]["n"]["v"] = new json_integer(3);
    js1["n"]["n"]["n"] = new json_object();
    js1["n"]["n"]["n"]["v"] = new json_integer(4);

    json js2(new json_object());
    js2["v"] = new json_integer(10);
    js2["n"] = new json_object();
    js2["n"]["v"] = new json_integer(20);
    js2["n"]["n"] = new json_object();
    js2["n"]["n"]["v"] = new json_integer(30);
    js2["n"]["n"]["n"] = new json_object();
    js2["n"]["n"]["n"]["v"] = new json_integer(40);

    json js3 = recursive_merge_with(js1, js2, (&add_json_integer));

    EXPECT_EQ(11,json_cast<int>(js3["v"]));
    EXPECT_EQ(22,json_cast<int>(js3["n"]["v"]));
    EXPECT_EQ(33,json_cast<int>(js3["n"]["n"]["v"]));
    EXPECT_EQ(44,json_cast<int>(js3["n"]["n"]["n"]["v"]));
  }
}

TEST(json, size)
{
  {
    json j;
    EXPECT_THROW(j.size(), json_bad_cast<size_t>);
  }
  {
    json j(new json_integer(0));
    EXPECT_THROW(j.size(), json_bad_cast<size_t>);
  }
  {
    json j(new json_float(0));
    EXPECT_THROW(j.size(), json_bad_cast<size_t>);
  }
  {
    json j(new json_bool(false));
    EXPECT_THROW(j.size(), json_bad_cast<size_t>);
  }
  {
    json j(new json_null);
    EXPECT_THROW(j.size(), json_bad_cast<size_t>);
  }
  {
    json j(new json_object);
    EXPECT_EQ(0, j.size());
    j["1"];
    j["2"];
    j["3"];
    EXPECT_EQ(3, j.size());
  }
  {
    json j(new json_array);
    EXPECT_EQ(0, j.size());
    j.add(json(new json_null));
    j.add(json(new json_integer(3)));
    EXPECT_EQ(2, j.size());
  }
}

TEST(json, type)
{
  {
    json j(new json_integer(0));
    EXPECT_EQ(json::Integer, j.type());
  }
  {
    json j(new json_float(0));
    EXPECT_EQ(json::Float, j.type());
  }
  {
    json j(new json_string(""));
    EXPECT_EQ(json::String, j.type());
  }
  {
    json j(new json_bool(false));
    EXPECT_EQ(json::Bool, j.type());
  }
  {
    json j(new json_array());
    EXPECT_EQ(json::Array, j.type());
  }
  {
    json j(new json_object());
    EXPECT_EQ(json::Object, j.type());
  }
  {
    json j(new json_null());
    EXPECT_EQ(json::Null, j.type());
  }
}

TEST(json, is)
{
  {
    json j(new json_integer(123));
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_TRUE(is<json_number>(j));
    EXPECT_TRUE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_float(3.14));
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_TRUE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_TRUE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_string("hell, world!"));
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_FALSE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_TRUE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_bool(true));
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_FALSE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_TRUE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_array());
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_FALSE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_TRUE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_object());
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_FALSE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_TRUE(is<json_object>(j));
    EXPECT_FALSE(is<json_null>(j));
  }
  {
    json j(new json_null());
    EXPECT_TRUE(is<json_value>(j));
    EXPECT_FALSE(is<json_number>(j));
    EXPECT_FALSE(is<json_integer>(j));
    EXPECT_FALSE(is<json_float>(j));
    EXPECT_FALSE(is<json_bool>(j));
    EXPECT_FALSE(is<json_string>(j));
    EXPECT_FALSE(is<json_array>(j));
    EXPECT_FALSE(is<json_object>(j));
    EXPECT_TRUE(is<json_null>(j));
  }
}

TEST(json, parse)
{
  {
    istringstream iss("1");
    json j;iss>>j;
    EXPECT_EQ(1, json_cast<int>(j));
  }
  {
    istringstream iss("123");
    json j;iss>>j;
    EXPECT_EQ(123, json_cast<int>(j));
  }
  {
    istringstream iss("1.234567e-10");
    json j;iss>>j;
    EXPECT_DOUBLE_EQ(1.234567e-10, json_cast<double>(j));
  }
  {
    istringstream iss("\"hello, \\u0022json!\\\"\"");
    json j; iss>>j;
    EXPECT_EQ("hello, \"json!\"", json_cast<string>(j));
  }
  {
    istringstream iss("false");
    json j;iss>>j;
    EXPECT_EQ(false, json_cast<bool>(j));
  }
  {
    istringstream iss("null");
    json j;iss>>j;
    EXPECT_TRUE(is<json_null>(j));
  }
  {
    istringstream iss("true");
    json j;iss>>j;
    EXPECT_EQ(true, json_cast<bool>(j));
  }

  {
    istringstream iss("[}");
    json j;
    EXPECT_THROW(iss>>j, jubatus::util::lang::parse_error);
  }

  {
    istringstream iss(
      "   {\n"
      "\"Image\": {\n"
      "\"Width\":  800,\n"
      "\"Height\": 600,\n"
      "\"Title\":  \"View from 15th Floor\",\n"
      "\"Thumbnail\": {\n"
      "\"Url\":    \"http://www.example.com/image/481989943\",\n"
      "\"Height\": 125,\n"
      "\"Width\":  \"100\"\n"
      "},\n"
      "\"IDs\": [116, 943, 234, 38793]\n"
      "}\n"
      "}"
      );

    json j; iss>>j;

    example1 v;
    v.Image.Width=800;
    v.Image.Height=600;
    v.Image.Title="View from 15th Floor";
    v.Image.Thumbnail.Url="http://www.example.com/image/481989943";
    v.Image.Thumbnail.Height=125;
    v.Image.Thumbnail.Width="100";
    v.Image.IDs.push_back(116);
    v.Image.IDs.push_back(943);
    v.Image.IDs.push_back(234);
    v.Image.IDs.push_back(38793);

    example1 w;
    from_json(j, w);

    EXPECT_EQ(true, v==w);
  }
}

TEST(json, bignum_parse)
{
  {
    // 36893488147419103233 == 2^65 + 1
    istringstream iss("36893488147419103233e0");
    json j;iss>>j;
    EXPECT_DOUBLE_EQ(36893488147419103233e0, json_cast<double>(j));
  }
}

TEST(json, pretty)
{
  {
    example1 v;
    v.Image.Width=800;
    v.Image.Height=600;
    v.Image.Title="View from 15th Floor";
    v.Image.Thumbnail.Url="http://www.example.com/image/481989943";
    v.Image.Thumbnail.Height=125;
    v.Image.Thumbnail.Width="100";
    v.Image.IDs.push_back(116);
    v.Image.IDs.push_back(943);
    v.Image.IDs.push_back(234);
    v.Image.IDs.push_back(38793);

    stringstream ss;
    ss<<pretty(to_json(v));

    example1 w;
    ss>>via_json(w);

    EXPECT_EQ(true, v==w);
  }

  {
    vector<example2> v;

    example2 a={
      "zip",
      37.7668,
      -122.3959,
      "",
      "SAN FRANCISCO",
      "CA",
      "94107",
      "US"
    };

    example2 b={
      "zip",
      37.371991,
      -122.026020,
      "",
      "SUNNYVALE",
      "CA",
      "94085",
      "US"
    };

    v.push_back(a);
    v.push_back(b);

    stringstream ss;
    ss<<pretty(to_json(v));

    vector<example2> w;
    ss>>via_json(w);

    EXPECT_EQ(true, v==w);
  }
}

TEST(json, clone)
{
  {
    example1 v;
    v.Image.Width=800;
    v.Image.Height=600;
    v.Image.Title="View from 15th Floor";
    v.Image.Thumbnail.Url="http://www.example.com/image/481989943";
    v.Image.Thumbnail.Height=125;
    v.Image.Thumbnail.Width="100";
    v.Image.IDs.push_back(116);
    v.Image.IDs.push_back(943);
    v.Image.IDs.push_back(234);
    v.Image.IDs.push_back(38793);

    json js1 = to_json(v);
    json js2 = js1;
    json js3 = js1.clone();

    js1["Image"]["IDs"][2] = new json_integer(42);
    js1["Image"]["IDs"].add(new json_integer(105));

    EXPECT_EQ(json_cast<int>(js1["Image"]["IDs"][2]), 42);
    EXPECT_EQ(json_cast<int>(js2["Image"]["IDs"][2]), 42);
    EXPECT_EQ(json_cast<int>(js3["Image"]["IDs"][2]), 234);

    EXPECT_EQ(js1["Image"]["IDs"].size(), 5);
    EXPECT_EQ(js2["Image"]["IDs"].size(), 5);
    EXPECT_EQ(js3["Image"]["IDs"].size(), 4);

    json js4 = js1["Image"]["Thumbnail"];
    json js5 = js1["Image"]["Thumbnail"].clone();
    js1["Image"]["Thumbnail"]["Height"] = new json_integer(200);

    EXPECT_EQ(json_cast<int>(js4["Height"]), 200);
    EXPECT_EQ(json_cast<int>(js5["Height"]), 125);
  }
}

TEST(json, utf8)
{
  string s="波浪ワールド";
  string j=lexical_cast<string>(to_json(s));
  EXPECT_EQ(j, "\"\\u6CE2\\u6D6A\\u30EF\\u30FC\\u30EB\\u30C9\"");
  string t;
  from_json<string>(lexical_cast<json>(j), t);
  EXPECT_EQ(s, t);
}

TEST(json, utf8_2)
{
  string s="{\"title\": \"坊ちゃん\"}";
  istringstream ss(s);
  json j; ss>>j;
  EXPECT_EQ(json_cast<string>(j["title"]), "坊ちゃん");
}

TEST(json, utf8_without_escaping)
{
  string s="波浪ワールド";
  string j=lexical_cast<string>(without_escape(to_json(s)));
  EXPECT_EQ(j, "\"波浪ワールド\"");
  string t;
  from_json<string>(lexical_cast<json>(j), t);
  EXPECT_EQ(s, t);
}

TEST(json, with_default)
{
  {
    json js;
    EXPECT_EQ(json_cast_with_default<int>(js), 0);
    EXPECT_EQ(json_cast_with_default<int>(js, 1), 1);
    EXPECT_DOUBLE_EQ(json_cast_with_default<double>(js), 0);
    EXPECT_DOUBLE_EQ(json_cast_with_default<double>(js, 3.14), 3.14);
    EXPECT_FLOAT_EQ(json_cast_with_default<float>(js), 0);
    EXPECT_FLOAT_EQ(json_cast_with_default<float>(js, 3.14), 3.14f);
    EXPECT_EQ(json_cast_with_default<string>(js), "");
    EXPECT_EQ(json_cast_with_default<string>(js, "hoge"), "hoge");
    EXPECT_EQ(json_cast_with_default<bool>(js), false);
    EXPECT_EQ(json_cast_with_default<bool>(js, true), true);
  }
  {
    json js;
    istringstream iss("{\"abc\": 987}");
    iss>>js;

    map<string, int> mm;
    mm["abc"]=123;
    mm["def"]=456;
    map<string, int> nn=json_cast_with_default(js, mm);

    mm["abc"]=987;
    EXPECT_EQ(mm==nn, true);
  }
  {
    example3 n;

    example3 a=json_cast_with_default<example3>(json());
    EXPECT_EQ(n==a, true);

    istringstream iss("{\"a\": 2}");
    example3 b;
    iss>>via_json_with_default(b);

    n.a=2;
    EXPECT_EQ(n==b, true);
  }
}

struct opt1{
  opt1(): abc(0), def() {}

  int abc;
  jubatus::util::data::optional<int> def;

  bool operator==(const opt1 &r) const{
    return abc==r.abc && def==r.def;
  }

  template <class Archive>
  void serialize(Archive &ar){
    ar & JUBA_MEMBER(abc) & JUBA_MEMBER(def);
  }

  bool is_json_rep(const string& s) const {
    if (def) {
      ostringstream oss1, oss2;
      oss1 << "{\"abc\":" << abc << ",\"def\":" << stringify_def() << "}";
      oss2 << "{\"def\":" << stringify_def() << ",\"abc\":" << abc << "}";
      return s == oss1.str() || s == oss2.str();
    } else {
      ostringstream oss;
      oss << "{\"abc\":" << abc << "}";
      return s == oss.str();
    }
  }

  string stringify_def() const {
    if (def) {
      return lexical_cast<string>(*def);
    } else {
      return "null";
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const opt1& x) {
    os << "abc: " << x.abc;
    if (x.def) {
      os << ", def: " << x.stringify_def();
    }
    return os;
  }
};
bool is_json_rep(const string& s, const opt1& x) {
  return x.is_json_rep(s);
}

TEST(json, optional)
{
  {
    istringstream iss("{\"abc\": 9}");
    opt1 a; iss>>via_json(a);
    opt1 b; b.abc=9;
    EXPECT_TRUE(a==b);
  }
  {
    istringstream iss("{\"abc\": 123, \"def\": 456}");
    opt1 a; iss>>via_json(a);
    opt1 b; b.abc=123; b.def=456;
    EXPECT_TRUE(a==b);
  }
  {
    EXPECT_THROW({
        istringstream iss("{}");
        opt1 a; iss>>via_json(a);
      },
      std::bad_cast);
    EXPECT_THROW({
        istringstream iss("{\"def\": 456}");
        opt1 a; iss>>via_json(a);
      },
      std::bad_cast);
    EXPECT_THROW({
        istringstream iss("{}");
        opt1 a; iss>>via_json(a);
      },
      json_bad_cast_any);
    EXPECT_THROW({
        istringstream iss("{\"def\": 456}");
        opt1 a; iss>>via_json(a);
      },
      json_bad_cast_any);
  }
  {
    opt1 a;
    a.abc=123;
    ostringstream oss; oss<<to_json(a);
    EXPECT_PRED2(&is_json_rep, oss.str(), a);
  }
  {
    opt1 a;
    a.abc=123;
    a.def=456;
    ostringstream oss; oss<<to_json(a);
    EXPECT_PRED2(&is_json_rep, oss.str(), a);
  }
}

TEST(json, cast_to_json)
{
  {
    json j=to_json(123);
    json k=json_cast<json>(j);
    
    EXPECT_EQ(json_cast<int>(k), 123);
  }

  {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    json j=to_json(v);
    vector<json> k=json_cast<vector<json> >(j);
    vector<int> w;
    for (size_t i=0; i<k.size(); i++)
      w.push_back(json_cast<int>(k[i]));

    EXPECT_TRUE(v==w);
  }
}

TEST(json, serialization)
{
  {
    ostringstream os;
    json_oarchive oa(os);

    oa<<123<<3.14<<string("hoge");
    EXPECT_EQ(os.str(), "123\n3.14\n\"hoge\"\n");
  }

  {
    ostringstream os;
    json_oarchive oa(os);

    vector<int> v;
    v.push_back(123);
    v.push_back(456);
    v.push_back(789);

    oa << v;
    EXPECT_EQ(os.str(), "[123,456,789]\n");
  }

  {
    stringstream ss;
    json_oarchive oa(ss);

    example1 v;
    v.Image.Width=800;
    v.Image.Height=600;
    v.Image.Title="View from 15th Floor";
    v.Image.Thumbnail.Url="http://www.example.com/image/481989943";
    v.Image.Thumbnail.Height=125;
    v.Image.Thumbnail.Width="100";
    v.Image.IDs.push_back(116);
    v.Image.IDs.push_back(943);
    v.Image.IDs.push_back(234);
    v.Image.IDs.push_back(38793);

    oa << v;

    jubatus::util::data::serialization::json_iarchive ia(ss);

    example1 w;
    ia >> w;

    EXPECT_TRUE(v==w);
  }
}

TEST(json, out_of_range)
{
  json j(new json_object());
  j["abc"]=json();

  const json &k=j;
  EXPECT_NO_THROW(k["abc"]);
  EXPECT_THROW(k["def"], std::out_of_range);

  EXPECT_NO_THROW(j["def"]);

  json l;
  EXPECT_THROW(l["ghi"], std::bad_cast);
}

TEST(json, parse_multiple_jsons)
{
#define JSON(...) #__VA_ARGS__
  string js=JSON(
  {\n
    "a":"b","c":"d"\n
  }\n
  \n
  {
    "abc":"def"
  } [1,2,3]\n\n\n\n\n\n\n\n\n\n\n\n\n
  );
#undef JSON

  std::istringstream in(js);
  json_parser parser(in);
  json j;
  EXPECT_NO_THROW({j=parser.parse();});
  EXPECT_NO_THROW({j["a"];j["c"];});
  EXPECT_EQ("b", json_cast<std::string>(j["a"]));
  EXPECT_EQ("d", json_cast<std::string>(j["c"]));
  EXPECT_NO_THROW({j=parser.parse();});
  EXPECT_NO_THROW({j["abc"];});
  EXPECT_EQ("def", json_cast<std::string>(j["abc"]));
  EXPECT_NO_THROW({j=parser.parse();});
  vector<int> v;
  EXPECT_NO_THROW({v = json_cast<std::vector<int> >(j);});
  ASSERT_EQ(3U, v.size());
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(2, v[1]);
  EXPECT_EQ(3, v[2]);
  EXPECT_THROW({j=parser.parse();}, jubatus::util::lang::end_of_data);
}

TEST(json, parse_invalid_multiple_json)
{
#define JSON(...) #__VA_ARGS__
  string js=JSON(
  {\n
    "a":"b","c":"d"\n
  }\n
  \n
  {
    "abc":{"recursive":"hoge"}
  }
  {
    "this":{"should":{"cause":"parse error",}}
  }
  );
#undef JSON

  std::istringstream in(js);
  json_parser parser(in);
  json j;
  EXPECT_NO_THROW({j=parser.parse();});
  EXPECT_NO_THROW({j=parser.parse();});
  EXPECT_THROW({j=parser.parse();}, jubatus::util::lang::parse_error);
}

TEST(json, escape_ctrl)
{
#define JSON(...) #__VA_ARGS__
  string js=JSON(
  "\u0003"
  );
#undef JSON

  std::istringstream in(js);
  json_parser parser(in);
  json j;
  EXPECT_NO_THROW({j=parser.parse();});
  EXPECT_EQ("\x03", json_cast<std::string>(j));
  std::ostringstream os;
  os<<pretty(j);
  EXPECT_EQ("\"\\u0003\"\n", os.str());
  std::ostringstream os2;
  os2<<j;
  EXPECT_EQ("\"\\u0003\"", os2.str());
}

TEST(json, invalid_json_num)
{
  { // LLONG_MAX + 1
    istringstream iss(std::string("9223372036854775808"));
    json j;
    EXPECT_THROW(iss>>j, jubatus::util::lang::parse_error);
  }

  { // LLONG_MIN - 1
    istringstream iss(std::string("-9223372036854775809"));
    json j;
    EXPECT_THROW(iss>>j, jubatus::util::lang::parse_error);
  }
}
