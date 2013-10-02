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

#include "aho_corasick.h"

#include <stack>
#include <algorithm>
#include <cstring>

using namespace std;

namespace pfi{
namespace data{
namespace string{

aho_corasick::aho_corasick(const vector<std::string> &words)
{
  vector<pair<std::string, int> > ixwords;
  for (size_t i=0;i<words.size();i++)
    ixwords.push_back(make_pair(words[i], i));
  sort(ixwords.begin(), ixwords.end());
  construct(ixwords);
}

void aho_corasick::construct(const vector<pair<std::string, int> > &words)
{
  if (words.empty()) return;

  // create double array trie
  int tbl[256]={};
  int children[256]={};
  int children_num=0;

  int bef_base=1;

  stack<pair<pair<int, int>, pair<int,int> > > ss;
  ss.push(make_pair(make_pair(0,0), make_pair(0,words.size())));

  nodes.resize(257);

  while(!ss.empty()){
    int pos=ss.top().first.first;
    int dep=ss.top().first.second;
    int ix=ss.top().second.first;
    int n=ss.top().second.second;
    ss.pop();

    memset(tbl, 0, sizeof(tbl));
    children_num=0;
    
    if ((int)words[ix].first.length()==dep)
      nodes[pos].output=words[ix].second;
    
    for (int i=0;i<n;i++){
      int c=dep<(int)words[ix+i].first.length()?(unsigned char)words[ix+i].first[dep]:0;
      if (tbl[c]++==0)
        children[children_num++]=c;
    }

    int base=max(1, bef_base-32);
    for (;;base++){
      bool ok=true;
      for (int ci=children[0]==0?1:0;ci<children_num;ci++){
        int i=children[ci];
        if (nodes[base+i].check>=0){
          ok=false;
          break;
        }
      }
      if (ok) break;
    }

    nodes[pos].base=base;
    for (int cur=0, ci=0;ci<children_num;ci++){
      int i=children[ci];
      if (i>0 && tbl[i]>0){
        int nix=nodes[pos].base+i;
        if (nix+256>=(int)nodes.size()) nodes.resize(nix+256);
        nodes[nix].check=pos;
        ss.push(make_pair(make_pair(nix, dep+1), make_pair(ix+cur, tbl[i])));
      }
      cur+=tbl[i];
    }

    bef_base=base;
  }

  // create failure link
  vector<int> q(nodes.size());
  int qh=0,qt=0;
  q[qt++]=0;
  while (qh!=qt) {
    int pos=q[qh++];
    if (pos>0 && nodes[nodes[pos].fail].output!=-1 && nodes[pos].output==-1)
      nodes[pos].output=-2;
    
    for (int i=0;i<256;++i) {
      if (nodes[nodes[pos].base+i].check!=pos)
        continue;

      int m=nodes[pos].fail;
      while (m>=0 && nodes[nodes[m].base+i].check!=m) m=nodes[m].fail;
      if (m>=0) nodes[nodes[pos].base+i].fail=nodes[m].base+i;
      else nodes[nodes[pos].base+i].fail=0;
      q[qt++]=nodes[pos].base+i;
    }
  }
  vector<node>(nodes).swap(nodes);
}

void aho_corasick::search(const std::string& s, vector<pair<int,int> >& res) const
{
  if (nodes.empty()) return;

  int pos=0;
  for (size_t i=0;i<s.size();++i) {
    int c=(unsigned char)s[i];
    while(pos>0 && nodes[nodes[pos].base+c].check!=pos) pos=nodes[pos].fail;
    if (nodes[nodes[pos].base+c].check!=pos) continue;
    pos=nodes[pos].base+c;

    if (nodes[pos].output!=-1) {
      int cur=pos;
      while(cur!=0) {
        if (nodes[cur].output>=0) {
          res.push_back(make_pair(nodes[cur].output,i+1)); // word ID and end word pos
        }
        cur=nodes[cur].fail;
      }
    }
  }
}

} // string
} // data
} // pficommon
