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

#ifndef JUBATUS_UTIL_DATA_INTERN_H_
#define JUBATUS_UTIL_DATA_INTERN_H_

#include <vector>

#include "unordered_map.h"
#include "serialization/unordered_map.h"

namespace jubatus {
namespace util {
namespace data {

/**
 * @brief Key to ID dictionary class
 */
template<typename Key,
         class Hash = hash<Key>,
         class EqualKey = std::equal_to<Key>,
         class Alloc = std::allocator<std::pair<const Key, int> > >
class intern {
  typedef unordered_map<Key, int, Hash, EqualKey, Alloc> map_t;

public:
  /**
   * @brief is it empty
   */
  bool empty() const {
    return tbl.empty();
  }

  /**
   * @brief clean contents
   */
  void clear() {
    tbl.clear();
    lbt.clear();
  }

  /**
   * @brief return the size of tbl
   */
  size_t size() const {
    return tbl.size();
  }

  /**
   * @brief get Key's ID
   * @param Key
   */
  int key2id_nogen(const Key& key) const {
    typename map_t::const_iterator it = tbl.find(key);
    if (it != tbl.end())
      return it->second;
    return -1;
  }

  /**
   * @brief get Key' ID
   * @param Key
   * @param create new entry if missing
   */
  int key2id(const Key& key, bool gen = true) {
    typename map_t::const_iterator it = tbl.find(key);
    if (it != tbl.end())
      return it->second;
    if (!gen)
      return -1;
    int id = tbl.size();
    tbl.insert(std::make_pair(key, id));
    lbt.push_back(key);
    return id;
  }

  /**
   * @brief get key from ID
   * @param ID
   */
  const Key& id2key(int id) const {
    return lbt[id];
  }

  /**
   * @brief return is key exist?
   */
  bool exist_key(const Key& key) const {
    return tbl.count(key);
  }
  /**
   * @brief return is id exist?
   */
  bool exist_id(int id) const {
    return id >= 0 && id < (int)lbt.size();
  }

  void swap(intern& other) {
    tbl.swap(other.tbl);
    lbt.swap(other.lbt);
  }

private:

  friend class jubatus::util::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    if (ar.is_read) {
      intern tmp;
      ar & tmp.tbl;
      tmp.lbt.resize(tmp.tbl.size());
      for (typename map_t::iterator it = tmp.tbl.begin(); it != tmp.tbl.end(); ++it)
        tmp.lbt[it->second] = it->first;
      swap(tmp);
    } else {
      ar & tbl;
    }
  }

  map_t tbl; // key to ID
  std::vector<Key> lbt;	   // ID to key
};

template <typename Key, class Hash, class EqualKey, class Alloc>
void swap(intern<Key, Hash, EqualKey, Alloc>& x, intern<Key, Hash, EqualKey, Alloc>& y)
{
  x.swap(y);
}

} // data
} // util
} // jubatus

#endif // #ifndef JUBATUS_UTIL_DATA_INTERN_H_
