#pragma once

#include <re2/re2.h>
#include <string>
#include <vector>
#include <pficommon/lang/scoped_ptr.h>
#include <map>

#include "../../server/fv_converter/word_splitter.hpp"

namespace jubatus {

class re2_splitter : jubatus::word_splitter {
 public:
  re2_splitter(const std::string& regexp, int group, int end);
  void split(const std::string& str,
             std::vector<std::pair<size_t, size_t> >& bounds);
 private:
  re2::RE2 re_;
  int group_;
  int end_;
};

extern "C" {
  jubatus::re2_splitter* create(const std::map<std::string, std::string>& args);
}

}
