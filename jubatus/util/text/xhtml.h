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

#ifndef JUBATUS_UTIL_TEXT_XHTML_H_
#define JUBATUS_UTIL_TEXT_XHTML_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../lang/shared_ptr.h"

namespace jubatus {
namespace util{
namespace text{
namespace xhtml{

std::string html_encode(const std::string &s);

class html_elem{
public:
  virtual ~html_elem(){}

  virtual void render(std::ostream &os) const =0;
  void pretty(std::ostream &os) const { pretty(os, 0); }

  virtual void pretty(std::ostream &os, int /* indent */) const { render(os); }
};

class tag_elem : public html_elem{
public:
  typedef std::map<std::string, std::string> attr_type;
  typedef std::vector<jubatus::util::lang::shared_ptr<html_elem> > children_type;

  tag_elem(const std::string &name): name_(name) {}
  tag_elem(const std::string &name, const attr_type &attr)
    : name_(name), attr_(attr) {}

  std::string &operator[](const std::string &key){
    return attr_[key];
  }

  void add_child(const jubatus::util::lang::shared_ptr<html_elem>& elem){
    children_.push_back(elem);
  }

  attr_type &attr(){ return attr_; }
  children_type &children(){ return children_; }

  const attr_type &attr() const { return attr_; }
  const children_type &children() const { return children_; }

  void render(std::ostream &os) const;
  void pretty(std::ostream &os, int indent) const;

private:
  std::string name_;
  attr_type attr_;
  
  children_type children_;
};

class text_elem : public html_elem{
public:
  text_elem(const std::string &text): text_(text) {}

  void render(std::ostream &os) const;

protected:

  std::string text_;
};

class prim_elem : public text_elem{
public:
  prim_elem(const std::string &text): text_elem(text) {}

  void render(std::ostream &os) const;
};

} // xhtml
} // text
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_TEXT_XHTML_H_
