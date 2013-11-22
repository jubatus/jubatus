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

#include "onig_match.hpp"

#include <string>
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

regexp_match::regexp_match(const std::string& regexp)
  : reg_(NULL) {
  const UChar* pattern = reinterpret_cast<const UChar*>(regexp.c_str());
  if (ONIG_NORMAL != onig_new(&reg_, pattern, pattern + regexp.size(),
        ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, NULL)) {
    throw JUBATUS_EXCEPTION(converter_exception("invalid regular expression"));
  }
}

regexp_match::~regexp_match() {
  if (reg_) {
    onig_free(reg_);
  }
}

bool regexp_match::match(const std::string& key) {
  const UChar* str = reinterpret_cast<const UChar*>(key.c_str());
  const UChar* end = str + key.size();
  return onig_match(reg_, str, end, str, NULL, ONIG_OPTION_NONE) >= 0;
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

