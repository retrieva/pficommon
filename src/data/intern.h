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

#pragma once

#include <vector>
#include <map>

#include "serialization.h"

namespace pfi {
namespace data {

/**
 * @brief Key to ID dictionary class
 */
template<typename _Key, typename _Compare = std::less<_Key> >
class intern  {
public:

  /**
   * @brief is it empty
   */
  bool empty() const {
    return tbl.size()==0&&lbt.size()==0;
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
  int key2id_nogen(const _Key& key) const {
    typename std::map<_Key, int>::const_iterator it=tbl.find(key);
    if (it!=tbl.end()) return it->second;
    return -1;
  }

  /**
   * @brief get Key' ID
   * @param Key
   * @param create new entry if missing
   */
  int key2id(const _Key& key, bool gen=true) {
    typename std::map<_Key, int>::const_iterator it=tbl.find(key);
    if (it!=tbl.end()) return it->second;
    if (gen==false) return -1;
    int id=tbl.size();
    tbl.insert(make_pair(key,id));
    lbt.push_back(key);
    return id;
  }

  /**
   * @brief get key from ID
   * @param ID
   */
  const _Key& id2key(int id) const {
    return lbt[id];
  }

  /**
   * @brief return is key exist?
   */
  bool exist_key(const _Key& key) const {
    return tbl.count(key)==true;
  }
  /**
   * @brief return is key exist?
   */
  bool exist_id(int id) const {
    return id<(int)lbt.size();
  }

private:

  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & tbl;
    if (ar.is_read) {
      lbt.clear();
      lbt.resize(tbl.size());
      for (typename std::map<_Key,int,_Compare>::iterator it=tbl.begin();it!=tbl.end();++it) {
	lbt[it->second]=it->first;
      }
    }
  }

  std::map<_Key,int,_Compare> tbl; // key to ID
  std::vector<_Key> lbt;	   // ID to key
};
} // data
} // pfi

