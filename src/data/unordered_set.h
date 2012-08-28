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

#ifndef INCLUDE_GUARD_PFI_DATA_UNORDERED_SET_H_
#define INCLUDE_GUARD_PFI_DATA_UNORDERED_SET_H_

#include "../pfi-config.h"

#if HAVE_UNORDERED_SET
#include <unordered_set>
#elif HAVE_TR1_UNORDERED_SET
#include <tr1/unordered_set>
#elif HAVE_EXT_HASH_SET
#include <ext/hash_set>
#else
#error "There is no unordered set implementation."
#endif

#include "functional_hash.h"

namespace pfi{
namespace data{

#if HAVE_UNORDERED_SET || HAVE_TR1_UNORDERED_SET

template <class _Value,
          class _Hash = hash<_Value>,
          class _Pred = std::equal_to<_Value>,
          class _Alloc = std::allocator<_Value> >
class unordered_set :
    public unordered_namespace::unordered_set<_Value, _Hash, _Pred, _Alloc> {
  typedef unordered_namespace::unordered_set<_Value, _Hash, _Pred, _Alloc> _Base;

public:
  explicit
  unordered_set(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_set(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

template <class _Value,
          class _Hash = hash<_Value>,
          class _Pred = std::equal_to<_Value>,
          class _Alloc = std::allocator<_Value> >
class unordered_multiset :
    public unordered_namespace::unordered_multiset<_Value, _Hash, _Pred, _Alloc> {
  typedef unordered_namespace::unordered_multiset<_Value, _Hash, _Pred, _Alloc> _Base;

public:
  explicit
  unordered_multiset(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_multiset(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

#elif HAVE_EXT_HASH_SET

template <class _Value,
          class _Hash = hash<_Value>,
          class _Pred = std::equal_to<_Value>,
          class _Alloc = std::allocator<_Value> >
class unordered_set :
    public __gnu_cxx::hash_set<_Value, _Hash, _Pred, _Alloc> {
  typedef __gnu_cxx::hash_set<_Value, _Hash, _Pred, _Alloc> _Base;

public:
  explicit
  unordered_set(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_set(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

template <class _Value,
          class _Hash = hash<_Value>,
          class _Pred = std::equal_to<_Value>,
          class _Alloc = std::allocator<_Value> >
class unordered_multiset :
    public __gnu_cxx::hash_multiset<_Value, _Hash, _Pred, _Alloc> {
  typedef __gnu_cxx::hash_multiset<_Value, _Hash, _Pred, _Alloc> _Base;

public:
  explicit
  unordered_multiset(typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(n, hf, eql, a) {}

  template <class _InputIterator>
  unordered_multiset(_InputIterator __f, _InputIterator __l,
                typename _Base::size_type n = 10,
                const typename _Base::hasher &hf = typename _Base::hasher(),
                const typename _Base::key_equal &eql = typename _Base::key_equal(),
                const typename _Base::allocator_type &a = typename _Base::allocator_type())
    : _Base(__f, __l, n, hf, eql, a) {}
};

#endif

} // data
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATA_UNORDERED_SET_H_
