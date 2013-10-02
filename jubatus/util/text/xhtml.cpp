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

#include "xhtml.h"

#include <sstream>
using namespace std;

namespace jubatus {
namespace util{
namespace text{
namespace xhtml{

string html_encode(const string &s)
{
  ostringstream oss;
  for (int i=0;i<(int)s.length();i++){
    if (s[i]=='&')
      oss<<"&amp;";
    else if (s[i]=='\"')
      oss<<"&quot;";
    else if (s[i]=='\'')
      oss<<"&#039;";
    else if (s[i]=='<')
      oss<<"&lt;";
    else if (s[i]=='>')
      oss<<"&gt;";
    else
      oss<<s[i];
  }
  return oss.str();
}

void tag_elem::render(ostream &os) const
{
  os<<'<'<<html_encode(name_);
  for (attr_type::const_iterator p=attr_.begin();
       p!=attr_.end();p++){
    if (p->second!="")
      os<<' '<<html_encode(p->first)<<"=\""<<html_encode(p->second)<<"\"";
    else
      os<<' '<<html_encode(p->first);
  }

  if (children_.size()==0){
    os<<" />";
  }
  else{
    os<<'>';
    for (int i=0;i<(int)children_.size();i++)
      children_[i]->render(os);
    os<<"</"<<html_encode(name_)<<'>';
  }
}

void tag_elem::pretty(ostream &os, int indent) const
{
  os<<'<'<<html_encode(name_);
  for (attr_type::const_iterator p=attr_.begin();
       p!=attr_.end();p++){
    if (p->second!="")
      os<<' '<<html_encode(p->first)<<"=\""<<html_encode(p->second)<<"\"";
    else
      os<<' '<<html_encode(p->first);
  }

  os<<endl;
  for (int i=0;i<indent*2+1;i++)
    os<<' ';

  if (children_.size()==0){
    os<<" />";
  }
  else{
    os<<'>';
    for (int i=0;i<(int)children_.size();i++)
      children_[i]->pretty(os, indent+1);
    os<<"</"<<html_encode(name_)<<endl;
    for (int i=0;i<indent*2+1;i++)
      os<<' ';
    os<<'>';
  }
}

void text_elem::render(ostream &os) const
{
  os<<html_encode(text_);
}

void prim_elem::render(ostream &os) const
{
  os<<text_;
}

} // xhtml
} // text
} // util
} // jubatus
