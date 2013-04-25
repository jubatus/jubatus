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

#include <gtest/gtest.h>
#include "dynamic_loader.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(dynamic_loader, trivial) {
  dynamic_loader l(LIBSPLITTER_SAMPLE);
  l.load_symbol("create");
  {
    dynamic_loader l2(LIBSPLITTER_SAMPLE);
    // destroctor of l2 call dlclose, but you can use l
  }
  l.load_symbol("create");
}

TEST(dynamic_loader, unknown_so_file) {
  EXPECT_THROW(dynamic_loader lo("unknown_file.so"), converter_exception);
}

TEST(dynamic_loader, unknown_function) {
  dynamic_loader l(LIBSPLITTER_SAMPLE);
  EXPECT_THROW(l.load_symbol("unknown"), converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
