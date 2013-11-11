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


#include "onig_splitter.hpp"
#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/cast.h>
#include "exception.hpp"

using pfi::lang::lexical_cast;
using jubatus::core::fv_converter::converter_exception;

namespace jubatus {
namespace core {
namespace fv_converter {

regexp_splitter::regexp_splitter(const std::string& regexp, int group)
    : reg_(NULL),
      group_(group) {
  if (group < 0) {
    throw JUBATUS_EXCEPTION(converter_exception("'group' must be positive"));
  }

  const UChar* pattern = reinterpret_cast<const UChar*>(regexp.data());
  if (ONIG_NORMAL != onig_new(&reg_, pattern, pattern + regexp.size(),
        ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, NULL)) {
    throw JUBATUS_EXCEPTION(converter_exception("invalid regular expression"));
  }

  const int num_capture = onig_number_of_captures(reg_);
  if (group > num_capture) {
    std::string msg = "regexp '" + regexp + "' only contains "
        + lexical_cast<std::string>(num_capture)
        + " groups, but 'group' is " + lexical_cast<std::string>(group);
    throw JUBATUS_EXCEPTION(converter_exception(msg));
  }
}

regexp_splitter::~regexp_splitter() {
  if (reg_) {
    onig_free(reg_);
  }
}

void regexp_splitter::split(
    const std::string& str,
    std::vector<std::pair<size_t, size_t> >& bounds) const {

  const UChar* head = reinterpret_cast<const UChar*>(str.data());
  const UChar* end = head + str.size();

  OnigRegion* region = onig_region_new();
  try {
    int cur = 0;
    while (head + cur < end) {
      int match
          = onig_match(reg_, head, end, head + cur, region, ONIG_OPTION_NONE);
      if (match < 0) {
        // did not match
        cur++;
        continue;
      }

      const int pos = region->beg[group_];
      const int len = region->end[group_] - pos;
      bounds.push_back(std::make_pair(pos, len));

      if (len > 0) {
        cur += len;
      } else {
        ++cur;
      }
    }
    onig_region_free(region, 1);
  } catch (...) {
    onig_region_free(region, 1);
    throw;
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
