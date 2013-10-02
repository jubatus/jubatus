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

#ifndef INCLUDE_GUARD_PFI_DATA_LRU_H_
#define INCLUDE_GUARD_PFI_DATA_LRU_H_

#include <stdexcept>
#include <map>
#include <set>
#include <cassert>
#include <stdint.h>

namespace pfi{
namespace data{

// Least Recently Used Cache in O(log(n))
// *** thread unsafe ***

template <class K, class V>
class lru{
public:
  /**
     @param size the size of the cache
     size should be > 0
   */
  explicit lru(int size)
    : max_size(size)
    , cur(0){
  }

  bool has(const K &key) const{
    return cache.count(key)>0;
  }

  const V &get(const K &key) /* this is not const! */ {
    if (cache.count(key)>0){
      touch(key);
      return cache.find(key)->second.first;
    }
    throw std::runtime_error("lru::get(): key is not found");
  }

  void set(const K &key, const V &val){
    if (cache.count(key)>0) return;

    while((int)cache.size()>=max_size){
      assert(access_time.size()==cache.size());
      const K &del_key=access_time.begin()->second;
      cache.erase(del_key);
      access_time.erase(access_time.begin());
    }

    uint64_t cnt=cur++;
    access_time.insert(std::make_pair(cnt, key));
    cache[key]=std::make_pair(val, cnt);
  }

  void touch(const K &key){
    typename std::map<K, std::pair<V, uint64_t> >::iterator p=cache.find(key);
    if (p!=cache.end()){
      access_time.erase(std::make_pair(p->second.second, key));
      uint64_t cnt=cur++;
      access_time.insert(std::make_pair(cnt, key));
      cache.find(key)->second.second=cnt;
    }
  }

  void remove(const K &key){
    uint64_t del_time=cache[key].second;
    access_time.erase(std::make_pair(del_time, key));
    cache.erase(key);
  }

  void clear(){
    cache.clear();
    access_time.clear();
  }

  V &operator[](const K &key){
    if (cache.count(key)>0) {
      touch(key);
    } else {
      set(key, V());
    }
    return cache.find(key)->second.first;
  }

private:
  int max_size;
  uint64_t cur;
  std::map<K, std::pair<V, uint64_t> > cache;
  std::multiset<std::pair<uint64_t, K> > access_time;
};

} // data
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATA_LRU_H_
