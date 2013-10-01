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


#include "onig_filter.hpp"
#include <string>
#include <sstream>
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

regexp_filter::regexp_filter(const std::string& regexp,
    const std::string& replace)
    : reg_(NULL),
      replace_(replace) {
  const UChar* pattern = reinterpret_cast<const UChar*>(regexp.c_str());
  if (ONIG_NORMAL != onig_new(&reg_, pattern, pattern + regexp.size(),
        ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, NULL)) {
    throw JUBATUS_EXCEPTION(converter_exception("invalid regular expression"));
  }
}

regexp_filter::~regexp_filter() {
  if (reg_) {
    onig_free(reg_);
  }
}

void regexp_filter::filter(
    const std::string& input, std::string& output) const {
  std::stringstream ss;

  const UChar* head = reinterpret_cast<const UChar*>(input.c_str());
  const UChar* cur = head, *end = head + input.size();

  do {
    int r = onig_match(reg_, head, end, cur, NULL, ONIG_OPTION_NONE);
    if (r <= 0) {
      cur++;
      continue;
    }

    if (head != cur) {
      ss.write(reinterpret_cast<const char*>(head), cur - head);
    }
    ss << replace_;

    cur += r;
    head = cur;
  } while (cur != end);

  // output when leave unmatched string
  if (head != cur) {
    ss.write(reinterpret_cast<const char*>(head), cur - head);
  }

  output = ss.str();
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
