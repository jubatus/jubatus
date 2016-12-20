// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2016 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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


#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
#include <gtest/gtest.h>

#include "jubatus/core/fv_converter/exception.hpp"
#include "image_feature.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

TEST(image_feature, trivial) {
  jubatus::plugin::fv_converter::image_feature im;
  cv::Mat img = cv::imread("test_input/jubatus.jpg");
  unsigned int correct = img.cols * img.rows * 3;
  // read a file
  std::ifstream ifs("test_input/jubatus.jpg");
  if (!ifs) {
    std::cerr << "cannot open : test_input/jubatus.jpg"  << std::endl;
    exit(1);
  }
  std::stringstream buffer;
  buffer << ifs.rdbuf();

  std::vector<std::pair<std::string, float> > ret_fv;
  im.add_feature("jubatus", buffer.str(), ret_fv);
  ASSERT_EQ(correct, ret_fv.size());
}

TEST(image_feature, Dense_Sampler) {
  cv::Mat img = cv::imread("test_input/jubatus.jpg");
  jubatus::plugin::fv_converter::image_feature im;
  unsigned int correct = (img.cols) * (img.rows);
  // read a file
  std::ifstream ifs("test_input/jubatus.jpg");
  if (!ifs) {
    std::cerr << "cannot open : test_input/jubatus.jpg"  << std::endl;
    exit(1);
  }
  std::vector<cv::KeyPoint> kp_vec;
  im.dense_sampler(img, 1, kp_vec);
  ASSERT_EQ(correct, kp_vec.size());
}

TEST(image_feature, resize) {
  jubatus::plugin::fv_converter::image_feature im("RGB", true);
  cv::Mat img = cv::imread("test_input/jubatus.jpg");
  unsigned int correct = 64 * 64 * 3;
  // read a file
  std::ifstream ifs("test_input/jubatus.jpg");
  if (!ifs) {
  std::cerr << "cannot open : test_input/jubatus.jpg" << std::endl;
  exit(1);
  }
  std::stringstream buffer;
  buffer << ifs.rdbuf();

  std::vector<std::pair<std::string, float> > ret_fv;
  im.add_feature("jubatus", buffer.str(), ret_fv);
  ASSERT_EQ(correct, ret_fv.size());
}

#if OPENCV_WITH_ORB
TEST(image_feature, ORB) {
  jubatus::plugin::fv_converter::image_feature im("ORB", true, 100, 100);
  cv::Mat img = cv::imread("test_input/jubatus.jpg");
  unsigned int compare = 0;
  // read a file
  std::ifstream ifs("test_input/jubatus.jpg");
  if (!ifs) {
    std::cerr << "cannot open : test_input/jubatus.jpg" << std::endl;
    exit(1);
  }
  std::stringstream buffer;
  buffer << ifs.rdbuf();

  std::vector<std::pair<std::string, float> > ret_fv;
  im.add_feature("jubatus", buffer.str(), ret_fv);
  ASSERT_LE(compare, ret_fv.size());
}
#endif

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
