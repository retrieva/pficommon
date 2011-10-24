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

namespace pfi{
namespace data{

#if HAVE_UNORDERED_MAP || HAVE_TR1_UNORDERED_MAP

template <class T>
class hash : public UOMAP_NS::hash<T> {};

#elif HAVE_EXT_HASH_MAP

template <class T>
class hash : public __gnu_cxx::hash<T> {};

#endif

} // data
} // pfi

#if HAVE_UNORDERED_MAP || HAVE_TR1_UNORDERED_MAP

#elif HAVE_EXT_HASH_MAP

namespace __gnu_cxx{

template <size_t=sizeof(size_t)>
struct fnv_hash{
  static size_t hash(const char *p, size_t length){
    size_t ret=0;
    for (; length>0; length--)
      ret=(ret*131)+*p++;
    return ret;
  }
};

template<>
struct fnv_hash<4>{
  static size_t hash(const char *p, size_t length){
    size_t ret=static_cast<size_t>(2166136261UL);
    for (; length>0; length--){
      ret^=static_cast<size_t>(*p++);
      ret*=static_cast<size_t>(16777619UL);
    }
    return ret;
  }
};

template<>
struct fnv_hash<8> {
  static size_t hash(const char *p, size_t length){
    size_t ret=static_cast<size_t>(14695981039346656037ULL);
    for (; length>0; length--){
      ret^=static_cast<size_t>(*p++);
      ret*=static_cast<size_t>(1099511628211ULL);
    }
    return ret;
  }
};

template <>
struct hash<std::string>{
  size_t operator()(const std::string &s) const{
    return fnv_hash<>::hash(s.c_str(), s.length());
  }
};

template<>
struct hash<float>{
  size_t operator()(float val) const{
    if (val!=0.0f)
      return fnv_hash<>::hash(reinterpret_cast<const char*>(&val),
			      sizeof(val));
    return 0;
  }
};
  
template<>
struct hash<double>{
  size_t operator()(double val) const{
    if (val!=0.0)
      return fnv_hash<>::hash(reinterpret_cast<const char*>(&val),
			      sizeof(val));
    return 0;
  }
};

template<>
struct hash<long double>{
  size_t operator()(long double val) const{
    if (val!=0.0L)
      return fnv_hash<>::hash(reinterpret_cast<const char*>(&val),
			      sizeof(val));
    return 0;
  }
};

} // __gnu_cxx

#endif
