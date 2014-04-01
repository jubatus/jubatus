// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "proceeded_stage.hpp"

#include <iostream>
#include <vector>
#include <gtest/gtest.h>

using std::vector;

namespace jubatus {
namespace core {
namespace burst {
namespace system {

class dummy_stage : public stage_base {
 public:
  explicit dummy_stage(const window* window)
      : window_(window) {
    std::cout << "proceeded_stage_test: dummy_stage construct" << std::endl;
  }
  // for unittest
  void set_window(window* window) {
    window_ = window;
  }

 private:
  bool is_drop(const window* w) const {
    std::cout << "proceeded_stage_test: start" << std::endl;
    return false;
  }

  void do_something(window* w) {
    ASSERT_EQ(window_->get_batch_interval(),
              w->get_batch_interval());
    ASSERT_EQ(window_->get_batch_size(),
              w->get_batch_size());
    ASSERT_EQ(window_->get_start_pos(),
              w->get_start_pos());
    ASSERT_EQ(window_->get_end_pos(),
              w->get_end_pos());
    for (int i = 0; i < window_->get_batch_interval(); i++) {
      ASSERT_EQ(window_->get_batches().at(i).get_d(),
                w->get_batches().at(i).get_d());
      ASSERT_EQ(window_->get_batches().at(i).get_r(),
                w->get_batches().at(i).get_r());
      ASSERT_EQ(window_->get_batches().at(i).get_batch_weight(),
                w->get_batches().at(i).get_batch_weight());
    }
  }

  void go_to_next_stage(window* w) {
    std::cout << "proceeded_stage_test: done." << std::endl;
  }

  // for unittest
  const window* window_;
};


TEST(proceeded_stage, general) {
  proceeded_stage pro(3);

  // data not pushed.
  window w0 = pro.get_result();

  window* w1 = new window(1998.5, 50, 10);
  w1->set_batch_weights(vector<double>(50, 5.8));
  w1->add_document(1567, 100, 1);

  dummy_stage* dummy1 = new dummy_stage(w1);
  pro.set_next(dummy1);
  pro.push(w1);

  window* w2 = new window(1999, 50, 10);
  pro.push(w2);

  window* w3 = new window(2299, 50, 10);
  dummy_stage* dummy3 = new dummy_stage(w3);
  pro.set_next(dummy3);
  pro.push(w3);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
