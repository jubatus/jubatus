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

#include "image_feature.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include OPENCV_HEADER

#include "jubatus/util/data/string/utility.h"
#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/core/fv_converter/util.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

image_feature::image_feature(
  const std::string& algorithm,
  const bool resize,
  float x_size,
  float y_size)
  : algorithm_(algorithm),
  resize_(resize),
  x_size_(x_size),
  y_size_(y_size) {
  if (x_size_ <= 0 || y_size_<= 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception("image size must be a positive number"));
  }
}

void image_feature::dense_sampler(
  const cv::Mat mat,
  const int step,
  std::vector<cv::KeyPoint>& kp_vec) const {
  for (int y = 0; y < mat.rows-step+1; y += step) {
    for (int x = 0; x < mat.cols-step+1; x += step) {
      kp_vec.push_back(cv::KeyPoint(static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(step)));
    }
  }
}

void image_feature::add_feature(
  const std::string& key,
  const std::string& value,
  std::vector<std::pair<std::string, float> >& ret_fv) const {
  std::vector<unsigned char> buf(value.begin(), value.end());

#if(CV_MAJOR_VERSION == 2)
  cv::Mat mat_orig = cv::imdecode(cv::Mat(buf), CV_LOAD_IMAGE_COLOR);
#else
  cv::Mat mat_orig = cv::imdecode(cv::Mat(buf), cv::IMREAD_COLOR);
#endif

  // mat resize and gray scale for DENSE sampling
  cv::Mat mat_resize;
  cv::Mat mat_gray;
  if (resize_) {
    float m_x = x_size_ / mat_orig.cols;
    float m_y = y_size_ / mat_orig.rows;
    cv::resize(mat_orig, mat_resize, cv::Size(), m_x , m_y);
  } else {
    cv::resize(mat_orig, mat_resize, cv::Size(), 1, 1);
  }


#if(CV_MAJOR_VERSION == 2)
  cv::cvtColor(mat_resize, mat_gray, CV_BGR2GRAY);
#else
  cv::cvtColor(mat_resize, mat_gray, cv::COLOR_BGR2GRAY);
#endif

  // feature extractors
  if (algorithm_ == "RGB") {
    for (int y = 0; y < mat_resize.rows; ++y) {
      for (int x = 0; x < mat_resize.cols; ++x) {
        const cv::Vec3b& vec = mat_resize.at<cv::Vec3b>(y, x);
        for (int c = 0; c < 3; ++c) {
          std::ostringstream oss;
          oss << key << '#' << algorithm_
              << '/' << x << '-' << y << '-' << c;
          float val = static_cast<float>(vec[c]) / 255.0;
          ret_fv.push_back(std::make_pair(oss.str(), val));
        }
      }
    }
  } else if (algorithm_ == "ORB") {
#if OPENCV_WITH_ORB
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> kp_vec;
    dense_sampler(mat_gray, 1, kp_vec);
#if(CV_MAJOR_VERSION == 2)
    cv::OrbDescriptorExtractor extractor;
    extractor.compute(mat_gray, kp_vec, descriptors);
#else
    cv::Ptr<cv::Feature2D> extractor = cv::ORB::create();
    extractor->compute(mat_gray, kp_vec, descriptors);
#endif
    for (int i = 0; i < descriptors.rows; ++i) {
      for (int j = 0; j < descriptors.cols; ++j) {
        std::ostringstream oss;
        int p = descriptors.at<uchar>(i, j);
        oss << key << '#' << algorithm_
            <<'/'<< i << "-" << j << "-" << p;
        ret_fv.push_back(std::make_pair(oss.str(), p));
      }
    }
#else
    throw JUBATUS_EXCEPTION(
      converter_exception(
      "plugin built without ORB support (requires OpenCV 2.3 or later)"));
#endif
  } else {
      throw JUBATUS_EXCEPTION(
        converter_exception(
        "input algorithm among these : RGB, ORB(opencv2.3 or later)"));
  }
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

extern "C" {
jubatus::plugin::fv_converter::image_feature* create(
    const std::map<std::string, std::string>& params) {
  using jubatus::util::lang::lexical_cast;
  using jubatus::core::fv_converter::get_with_default;

  std::string algorithm =
      get_with_default(params, "algorithm", "RGB");
  std::string resize_str =
      get_with_default(params, "resize", "false");
  float x_size =
      lexical_cast<float>(get_with_default(params, "x_size", "64.0"));
  float y_size =
      lexical_cast<float>(get_with_default(params, "y_size", "64.0"));

  if (resize_str != "true" && resize_str != "false") {
    throw JUBATUS_EXCEPTION(jubatus::core::fv_converter::converter_exception(
    "resize must be a boolean value"));
  }
  bool resize = (resize_str == "true");
  return new jubatus::plugin::fv_converter::image_feature(
    algorithm, resize, x_size, y_size);
}
std::string version() {
  return JUBATUS_VERSION;
}
}  // extern "C"
