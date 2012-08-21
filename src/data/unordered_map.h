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

#ifndef INCLUDE_GUARD_PFI_DATA_UNORDERED_MAP_H_
#define INCLUDE_GUARD_PFI_DATA_UNORDERED_MAP_H_

#include "../pfi-config.h"

#if HAVE_UNORDERED_MAP
#include <unordered_map>
#elif HAVE_TR1_UNORDERED_MAP
#include <tr1/unordered_map>
#elif HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else
#error There is no unordered map implementation.
#endif

#if HAVE_UNORDERED_MAP
#define UOMAP_NS std
#else
#define UOMAP_NS std::tr1
#endif

#include "functional_hash.h"

namespace pfi{
namespace data{

#if HAVE_UNORDERED_MAP || HAVE_TR1_UNORDERED_MAP

template <class _Key, class _Tp,
          class _Hash = hash<_Key>,
          class _EqualKey = std::equal_to<_Key>,
          class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
class unordered_map :
    public UOMAP_NS::unordered_map<_Key, _Tp, _Hash, _EqualKey, _Alloc> {
  typedef UOMAP_NS::unordered_map<_Key, _Tp, _Hash, _EqualKey, _Alloc> _Base;

public:
  explicit
  unordered_map(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_map(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

template <class _Key, class _Tp,
          class _Hash = hash<_Key>,
          class _EqualKey = std::equal_to<_Key>,
          class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
class unordered_multimap :
    public UOMAP_NS::unordered_multimap<_Key, _Tp, _Hash, _EqualKey, _Alloc> {
  typedef UOMAP_NS::unordered_multimap<_Key, _Tp, _Hash, _EqualKey, _Alloc> _Base;

public:
  explicit
  unordered_multimap(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_multimap(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

#elif HAVE_EXT_HASH_MAP

template <class _Key, class _Tp,
          class _Hash = hash<_Key>,
          class _EqualKey = std::equal_to<_Key>,
          class _Alloc = std::allocator<_Tp> >
class unordered_map :
    public __gnu_cxx::hash_map<_Key, _Tp, _Hash, _EqualKey, _Alloc> {
  typedef __gnu_cxx::hash_map<_Key, _Tp, _Hash, _EqualKey, _Alloc> _Base;

public:
  explicit
  unordered_map(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_map(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

template <class _Key, class _Tp,
          class _Hash = hash<_Key>,
          class _EqualKey = std::equal_to<_Key>,
          class _Alloc = std::allocator<_Tp> >
class unordered_multimap :
    public __gnu_cxx::hash_multimap<_Key, _Tp, _Hash, _EqualKey, _Alloc> {
  typedef __gnu_cxx::hash_multimap<_Key, _Tp, _Hash, _EqualKey, _Alloc> _Base;

public:
  explicit
  unordered_multimap(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_multimap(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

#endif

#undef UOMAP_NS

} // data
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATA_UNORDERED_MAP_H_
