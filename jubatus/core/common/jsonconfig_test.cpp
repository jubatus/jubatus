// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>

#include "jsonconfig.hpp"

using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;
using pfi::lang::lexical_cast;
using pfi::text::json::json;
using pfi::text::json::json_array;
using pfi::text::json::json_bool;
using pfi::text::json::json_float;
using pfi::text::json::json_integer;
using pfi::text::json::json_object;
using pfi::text::json::json_string;

namespace jubatus {
namespace core {
namespace jsonconfig {

TEST(jsonconfig, empty) {
  EXPECT_EQ(pfi::text::json::json::Null, config().get().type());
}

TEST(jsonconfig, bool) {
  json j(new json_bool(false));
  EXPECT_EQ(false, config(j).As<bool>());
}

TEST(jsonconfig, bool_type_error) {
  try {
    json j(new json_bool(false));
    config(j).As<int>();
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Bool, e.actual());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, int) {
  json j(new json_integer(10));
  EXPECT_EQ(10, config(j).As<int>());
}

TEST(jsonconfig, int_type_error) {
  try {
    json j(new json_integer(10));
    config(j).As<std::string>();
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Integer, e.actual());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, float) {
  json j(new json_float(0.1));
  EXPECT_FLOAT_EQ(0.1, config(j).As<float>());
}

TEST(jsonconfig, float_type_error) {
  try {
    json j(new json_float(0.1));
    config(j).As<std::string>();
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Float, e.actual());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, array) {
  json j(new json_array());
  j.add(json(new json_integer(10)));
  EXPECT_EQ(10, config(j)[0].As<int>());
}

TEST(jsonconfig, array_out_of_range) {
  json j(new json_array());
  try {
    config(j)[1].As<int>();
    FAIL();
  } catch (const out_of_range& e) {
    EXPECT_EQ(0u, e.size());
    EXPECT_EQ(1u, e.position());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, array_type_error) {
  json j(new json_integer(10));
  config c(j);
  try {
    c[1];
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Array, e.expect());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, object) {
  json j(new json_object());
  j["age"] = json(new json_integer(10));
  EXPECT_EQ(10, config(j)["age"].As<int>());
}

TEST(jsonconfig, object_not_found) {
  json j(new json_object());
  j["age"] = json(new json_integer(10));
  config c(j);
  try {
    c["name"];
    FAIL();
  } catch (const not_found& e) {
    EXPECT_EQ("name", e.key());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, object_type_error) {
  json j(new json_integer(10));
  config c(j);
  try {
    c["age"];
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Object, e.expect());
    EXPECT_EQ("", e.path());
  }
}

TEST(jsonconfig, error_path) {
  json j(new json_array());
  j.add(json(new json_object()));
  j[0]["user"] = json(new json_object());
  j[0]["user"]["age"] = json(new json_integer(10));
  config c(j);
  try {
    c[0]["user"]["age"].As<std::string>();
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ("[0].user.age", e.path());
  }
}

TEST(jsonconfig_cast, int) {
  json j(new json_integer(1));
  config conf(j);
  EXPECT_EQ(1, config_cast<int>(conf));
}

TEST(jsonconfig_cast, float) {
  json j(new json_float(1.0));
  config conf(j);
  EXPECT_EQ(1.0, config_cast<float>(conf));
}

TEST(jsonconfig_cast, int_to_float) {
  json j(new json_integer(1));
  config conf(j);
  EXPECT_EQ(1.0, config_cast<float>(conf));
}

TEST(jsonconfig_cast, string) {
  json j(new json_string("test"));
  config conf(j);
  EXPECT_EQ("test", config_cast<string>(conf));
}

TEST(jsonconfig_cast, vector_int) {
  config conf(lexical_cast<json>("[1,2,3]"));
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  EXPECT_EQ(v, config_cast<vector<int> >(conf));
}

TEST(jsonconfig_cast, map) {
  config conf(lexical_cast<json>("{\"height\": 160, \"weight\": 60}"));
  map<string, int> m;
  m["height"] = 160;
  m["weight"] = 60;

  EXPECT_EQ(m, (config_cast<map<string, int> >(conf)));
}

TEST(jsonconfig_cast, unordered_map) {
  config conf(lexical_cast<json>("{\"height\": 160, \"weight\": 60}"));
  pfi::data::unordered_map<string, int> m;
  m["height"] = 160;
  m["weight"] = 60;

  pfi::data::unordered_map<string, int> v =
    config_cast<pfi::data::unordered_map<string, int> >(conf);
  EXPECT_EQ(m["height"], v["height"]);
  EXPECT_EQ(m["weight"], v["weight"]);
}

TEST(jsonconfig_cast, error_path) {
  config conf(lexical_cast<json>("{\"value\": [1,2,\"test\"]}"));
  try {
    config_cast<map<string, vector<int> > >(conf);
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(".value[2]", e.path());
    EXPECT_EQ(json::String, e.actual());
    EXPECT_EQ(json::Integer, e.expect());
  }
}

// epxect object[map<K, V>] but [array]
TEST(jsonconfig_cast, error_convert_map) {
  config conf(lexical_cast<json>("{\"value\": []}"));
  try {
    config_cast<map<string, map<string, string> > >(conf);
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(".value", e.path());
    EXPECT_EQ(json::Array, e.actual());
    EXPECT_EQ(json::Object, e.expect());
  }
}

// epxect object[unordered_map<K, V>] but [array]
TEST(jsonconfig_cast, error_convert_unordered_map) {
  config conf(lexical_cast<json>("{\"value\": []}"));
  try {
    config_cast<map<string, pfi::data::unordered_map<string, string> > >(conf);
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(".value", e.path());
    EXPECT_EQ(json::Array, e.actual());
    EXPECT_EQ(json::Object, e.expect());
  }
}

struct opt1 {
  int abc;
  pfi::data::optional<int> def;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(abc) & MEMBER(def);
  }
};

TEST(jsonconfig_cast, named_not_found) {
  config conf(lexical_cast<json>("{\"def\": 456}"));

  try {
    opt1 a = config_cast<opt1>(conf);
    FAIL();
  } catch (const not_found& e) {
    EXPECT_EQ("", e.path());
    EXPECT_EQ("abc", e.key());
  }
}

TEST(jsonconfig_cast, named_type_null) {
  config conf(lexical_cast<json>("{\"abc\" : null }"));

  try {
    config_cast<opt1>(conf);
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(".abc", e.path());
    EXPECT_EQ(json::Null, e.actual());
    EXPECT_EQ(json::Integer, e.expect());
  }
}

TEST(jsonconfig_cast, named_optional_nullable) {
  config conf(lexical_cast<json>("{\"abc\": 123, \"def\": null}"));
  ASSERT_NO_THROW(config_cast<opt1>(conf));
}

struct opt2 {
  int abc;
  pfi::data::optional<int> def;

  struct test {
    int a;
    int b;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(a) & MEMBER(b);
    }
  } test;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(abc) & MEMBER(def) & MEMBER(test);
  }
};

TEST(jsonconfig_cast, named_nest_not_found) {
  config conf(lexical_cast<json>("{\"abc\" : 124 }"));

  try {
    config_cast<opt2>(conf);
    FAIL();
  } catch (const not_found& e) {
    EXPECT_EQ("", e.path());
    EXPECT_EQ("test", e.key());
  }
}

struct opt3 {
  int abc;
  json conf;  // any json type

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(abc) & MEMBER(conf);
  }
};

TEST(jsonconfig_cast, json) {
  config conf(lexical_cast<json>("{\"abc\" : 124, \"conf\": {} }"));
  ASSERT_NO_THROW(config_cast<opt3>(conf));
}

TEST(jsonconfig_cast, json_null) {
  config conf(lexical_cast<json>("{\"abc\" : 124, \"conf\": null }"));
  ASSERT_NO_THROW(config_cast<opt3>(conf));
}

struct opt4 {
  int abc;
  config conf;  // any config

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(abc) & MEMBER(conf);
  }
};

TEST(jsonconfig_cast, confg) {
  config conf(lexical_cast<json>("{\"abc\" : 124, \"conf\": {} }"));
  EXPECT_NO_THROW(config_cast<opt4>(conf));
}

TEST(jsonconfig_cast, named_bad_type) {
  config conf(lexical_cast<json>("{\"abc\" : 124, \"test\": [] }"));

  try {
    config_cast<opt2>(conf);
    FAIL();
  } catch (const type_error& e) {
    EXPECT_EQ(json::Array, e.actual());
  }
}

struct Person {
  string name;
  double height;
  int age;
  map<string, string> attributes;
  pfi::data::optional<string> sport;
  pfi::data::optional<string> hobby;

  bool operator ==(const Person& p) const {
    return name == p.name && height == p.height && age == p.age
        && attributes == p.attributes && sport == p.sport && hobby == p.hobby;
  }

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(name) & MEMBER(height) & MEMBER(age) & MEMBER(attributes)
        & MEMBER(sport) & MEMBER(hobby);
  }
};

TEST(jsonconfig_cast, struct) {
  config conf(lexical_cast<json>(
      "{\"name\": \"Taro\", \"height\": 160.0, \"age\": 20, "
      "\"attributes\": {\"address\": \"Tokyo\"}, \"sport\": \"tennis\"}"));
  Person p;
  p.name = "Taro";
  p.height = 160.0;
  p.age = 20;
  p.attributes["address"] = "Tokyo";
  p.sport = "tennis";

  EXPECT_EQ(p, config_cast<Person>(conf));
}

struct server_conf {
  struct web_conf {
    std::string host;
    int port;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(host) & MEMBER(port);
    }
  } web_server;

  std::vector<std::string> users;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(web_server) & MEMBER(users);
  }
};

TEST(jsonconfig_cast, config_cast_error) {
  config conf(lexical_cast<json>(
      "{\"web_server\": { \"host\" : 123}, \"users\": [\"abc\", 1] }"));

  config_error_list errors;
  server_conf c = config_cast<server_conf>(conf, errors);

  for (size_t i = 0; i < errors.size(); i++) {
    cout << errors[i]->what() << endl;
  }

  EXPECT_EQ(3U, errors.size());

  type_error* e1 = dynamic_cast<type_error*>(errors[0].get());
  ASSERT_TRUE(e1);
  EXPECT_EQ(".web_server.host", e1->path());
  EXPECT_EQ(json::Integer, e1->actual());

  not_found* e2 = dynamic_cast<not_found*>(errors[1].get());
  ASSERT_TRUE(e2);
  EXPECT_EQ(".web_server", e2->path());
  EXPECT_EQ("port", e2->key());

  type_error* e3 = dynamic_cast<type_error*>(errors[2].get());
  ASSERT_TRUE(e3);
  EXPECT_EQ(".users[1]", e3->path());
  EXPECT_EQ(json::Integer, e3->actual());
}

TEST(jsonconfig_cast, cast_check_error) {
  config conf(lexical_cast<json>(
      "{\"web_server\": { \"host\" : 123}, \"users\": [\"abc\", 1] }"));

  try {
    config_cast_check<server_conf>(conf);
    FAIL();
  } catch (const cast_check_error& e) {
    const config_error_list& errors = e.errors();
    EXPECT_EQ(3, errors.size());

    type_error* e1 = dynamic_cast<type_error*>(errors[0].get());
    ASSERT_TRUE(e1);
    EXPECT_EQ(".web_server.host", e1->path());
    EXPECT_EQ(json::Integer, e1->actual());

    // same code of the 'config_cast_error'
  }
}

}  // namespace jsonconfig
}  // namespace core
}  // namespace jubatus
