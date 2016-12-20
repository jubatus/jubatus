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

#ifndef JUBATUS_PLUGIN_FV_CONVERTER_IMAGE_FEATURE_HPP_
#define JUBATUS_PLUGIN_FV_CONVERTER_IMAGE_FEATURE_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include OPENCV_HEADER

#include "jubatus/core/fv_converter/binary_feature.hpp"
#include "jubatus/core/fv_converter/exception.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::converter_exception;

class image_feature:public jubatus::core::fv_converter::binary_feature {
  public:
    virtual ~image_feature() {}
    image_feature(
      const std::string& algorithm = "RGB",
      const bool resize = false,
      const float x_size = 64.0,
      const float y_size = 64.0);
    void add_feature(
      const std::string& key,
      const std::string& value,
      std::vector<std::pair<std::string, float> >& ret_fv) const;
    void dense_sampler(
      const cv::Mat mat,
      const int step,
      std::vector<cv::KeyPoint>& keypoint) const;

  private:
    std::string algorithm_;
    bool resize_;
    float x_size_;
    float y_size_;
  };

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus


extern "C" {
jubatus::plugin::fv_converter::image_feature* create(
  const std::map<std::string, std::string>& params);
std::string version();
}  // extern "C"

#endif  // JUBATUS_PLUGIN_FV_CONVERTER_IMAGE_FEATURE_HPP_
