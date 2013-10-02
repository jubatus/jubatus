// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef JUBATUS_UTIL_DATA_CONFIG_FILE_H_
#define JUBATUS_UTIL_DATA_CONFIG_FILE_H_

#include <istream>
#include <string>
#include <map>

namespace jubatus {
namespace util {
namespace data {

class config_file {
public:
  explicit config_file(const std::string& filename);
  explicit config_file(std::istream& is);

  class section {
  public:
    typedef std::map<std::string, std::string>::iterator iterator;
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    section() {}

    std::string& operator[](const std::string& key) {
      return dat[key];
    }
    const std::string& operator[](const std::string& key) const {
      static const std::string empty("");
      const_iterator p = dat.find(key);
      if (p != dat.end())
        return p->second;
      return empty;
    }

    iterator find(const std::string& key) { return dat.find(key); }
    const_iterator find(const std::string& key) const { return dat.find(key); }

    iterator begin() { return dat.begin(); }
    iterator end() { return dat.end(); }

    const_iterator begin() const { return dat.begin(); }
    const_iterator end() const { return dat.end(); }

  private:
    std::map<std::string, std::string> dat;
  };

  typedef std::map<std::string, section>::iterator iterator;
  typedef std::map<std::string, section>::const_iterator const_iterator;

  section& operator[](const std::string& sec) {
    return dat[sec];
  }

  const section& operator[](const std::string& sec) const {
    static const section empty;
    const_iterator p = dat.find(sec);
    if (p != dat.end())
      return p->second;
    return empty;
  }

  iterator find(const std::string& sec) { return dat.find(sec); }
  const_iterator find(const std::string& sec) const { return dat.find(sec); }

  iterator begin() { return dat.begin(); }
  iterator end() { return dat.end(); }

  const_iterator begin() const { return dat.begin(); }
  const_iterator end() const { return dat.end(); }

private:
  void init(const std::string& filename, std::istream& is);

  std::map<std::string, section> dat;
};

} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_CONFIG_FILE_H_
