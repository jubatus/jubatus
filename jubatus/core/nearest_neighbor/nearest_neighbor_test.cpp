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

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <gtest/gtest.h>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/lang/cast.h"
#include "../common/jsonconfig.hpp"
#include "nearest_neighbor_base.hpp"
#include "nearest_neighbor_factory.hpp"

using std::map;
using std::string;
using std::vector;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace nearest_neighbor {

namespace {

// utility to write map<string, string> value in an expression
class make_config_type {
 public:
  make_config_type& operator()(const string& key, const string& value) {
    config_.insert(make_pair(key, value));
    return *this;
  }

  map<string, string> operator()() {
    map<string, string> ret;
    ret.swap(config_);
    return ret;
  }

 private:
  map<string, string> config_;
} make_config;

}  // namespace

class nearest_neighbor_test
    : public ::testing::TestWithParam<map<string, string> > {
 protected:
  void SetUp() {
    try {
      map<string, string> param = GetParam();
      string name = param["nearest_neighbor:name"];
      param.erase("nearest_neighbor:name");
      using jubatus::util::text::json::json;
      json config_js(new jubatus::util::text::json::json_object);
      for (map<string, string>::iterator it = param.begin();
           it != param.end(); ++it) {
        config_js.add(
            it->first,
            json(new jubatus::util::text::json::json_integer(
                jubatus::util::lang::lexical_cast<int>(it->second))));
      }

      using common::jsonconfig::config;

      table_.reset(new table::column_table);
      nn_ = create_nearest_neighbor(
          name, config(config_js, ""), table_, "localhost");
    } catch (common::jsonconfig::cast_check_error& e) {
      std::cout << "In Setup():" <<e.what() << '\n';
      vector<shared_ptr<common::jsonconfig::config_error> > v = e.errors();
      for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i]->what() << '\n';
      }
      throw;
    }
  }

  table::column_table* get_table() {
    return table_.get();
  }
  nearest_neighbor_base* get_nn() {
    return nn_.get();
  }

 private:
  shared_ptr<table::column_table> table_;
  shared_ptr<nearest_neighbor_base> nn_;
};

TEST_P(nearest_neighbor_test, type) {
  map<string, string> param = GetParam();
  const string name = param["nearest_neighbor:name"];
  nearest_neighbor_base* nn = get_nn();
  EXPECT_EQ(name, nn->type());
}

TEST_P(nearest_neighbor_test, empty_get_all_row_ids) {
  nearest_neighbor_base* nn = get_nn();

  vector<string> ids;
  nn->get_all_row_ids(ids);
  EXPECT_TRUE(ids.empty());
}

TEST_P(nearest_neighbor_test, get_all_row_ids) {
  nearest_neighbor_base* nn = get_nn();

  vector<string> expect;
  expect.push_back("jubatus");
  expect.push_back("kyun");
  expect.push_back("hah hah");

  for (int count = 0; count < 2; ++count) {  // duplicated set_row
    for (size_t i = 0; i < expect.size(); ++i) {
      nn->set_row(expect[i], common::sfv_t());
    }
  }

  vector<string> actual;
  nn->get_all_row_ids(actual);

  EXPECT_EQ(expect, actual);
}

TEST_P(nearest_neighbor_test, empty_neighbor_row) {
  nearest_neighbor_base* nn = get_nn();

  vector<std::pair<string, float> > ids;
  nn->neighbor_row("", ids, 1);
  EXPECT_TRUE(ids.empty());

  nn->neighbor_row(common::sfv_t(), ids, 1);
  EXPECT_TRUE(ids.empty());
}

// TODO(beam2d): Write approximated test of neighbor_row().

const map<string, string> configs[] = {
  make_config("nearest_neighbor:name", "lsh")("hash_num", "64")(),
  make_config("nearest_neighbor:name", "minhash")("hash_num", "64")(),
  make_config(
      "nearest_neighbor:name", "euclid_lsh")(
      "hash_num", "64")()
};

INSTANTIATE_TEST_CASE_P(
    lsh_test,
    nearest_neighbor_test,
    ::testing::ValuesIn(configs));

}  // namespace nearest_neighbor
}  // namespace core
}  // namespace jubatus
