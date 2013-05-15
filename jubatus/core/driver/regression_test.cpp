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

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "../storage/local_storage.hpp"
#include "../regression/regression.hpp"
#include "../regression/regression_test_util.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"
#include "regression.hpp"

#include "test_util.hpp"

using std::vector;
using std::pair;
using std::make_pair;
using std::string;
using std::cout;
using std::endl;
using pfi::lang::lexical_cast;
using jubatus::core::fv_converter::datum;

namespace jubatus {
namespace core {
namespace driver {

class regression_test : public ::testing::Test {
 protected:
  void SetUp() {
    core::storage::storage_base* storage = new core::storage::local_storage;
    core::regression::passive_aggressive::config config;
    config.C = std::numeric_limits<float>::max();
    config.epsilon = 0.1f;
    regression_.reset(
      new core::driver::regression(
        storage,
        new core::regression::passive_aggressive(config, storage),
        make_fv_converter()));
  }

  void TearDown() {
    regression_.reset();
  }

  void my_test();

  pfi::lang::shared_ptr<core::driver::regression> regression_;
};

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.num_values_.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<float, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<float, vector<double> > p = gen_random_data(1.0, 1.0, 10);
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

void regression_test::my_test() {
  const size_t example_size = 1000;

  vector<pair<float, datum> > data;
  make_random_data(data, example_size);
  for (size_t i = 0; i < example_size; i++) {
    regression_->train(data[i]);
  }

  vector<float> values, result;
  {
    vector<pair<float, datum> >::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
      values.push_back(it->first);
      result.push_back(regression_->estimate(it->second));
    }
  }

  ASSERT_EQ(example_size, result.size());
  ASSERT_EQ(data.size(), result.size());

  vector<float>::const_iterator it0;  // answers
  vector<float>::const_iterator it;
  size_t count = 0;
  for (it = result.begin(), it0 = values.begin();
      it != result.end() && it0 != values.end(); ++it, ++it0) {
    if (fabs(*it0 - *it) < 2.0) {
      count++;
    }
  }

  // num of wrong classification should be less than 1%
  EXPECT_GE(count, result.size() - 10);
}

TEST_F(regression_test, pa) {
  my_test();
}

TEST_F(regression_test, small) {
  cout << "train" << endl;
  datum d;
  d.num_values_.push_back(make_pair("f1", 1.0));
  pair<float, datum> data(10, d);
  regression_->train(data);

  std::string save_data;

  save_model(regression_->get_mixable_holder(), save_data);
  load_model(regression_->get_mixable_holder(), save_data);

  cout << "estimate" << endl;
  float res = regression_->estimate(d);
  cout << res << endl;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
