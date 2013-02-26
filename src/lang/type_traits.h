// Copyright (c)2008-2013, Preferred Infrastructure Inc.
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

#ifndef INCLUDE_GUARD_PFI_LANG_TYPE_TRAITS_H_
#define INCLUDE_GUARD_PFI_LANG_TYPE_TRAITS_H_

#include <stddef.h>

namespace pfi {
namespace lang {

template <class T, T v>
struct integral_constant {
  typedef T value_type;
  typedef integral_constant type;

  static const T value = v;

  operator T() const { return value; }
};

template <class T, T v>
const T integral_constant<T, v>::value;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;


namespace detail {
template <class>
struct is_void_helper : false_type {};

template <>
struct is_void_helper<void> : true_type {};
} // namespace detail

template <class>
struct remove_cv;

template <class T>
struct is_void : integral_constant<bool, detail::is_void_helper<typename remove_cv<T>::type>::value> {};


template <class T>
struct is_array : false_type {};

template <class T>
struct is_array<T[]> : true_type {};

template <class T, size_t n>
struct is_array<T[n]> : true_type {};


template <class T>
struct is_pointer_helper : false_type {};

template <class T>
struct is_pointer_helper<T*> : true_type {};

template <class T>
struct is_pointer : is_pointer_helper<typename remove_cv<T>::type> {};


template <class T>
struct is_lvalue_reference : false_type {};

template <class T>
struct is_lvalue_reference<T&> : true_type {};


// TODO: is_rvalue_reference
// TODO: is_member_object_pointer
// TODO: is_member_function_pointer
// TODO: is_enum
// TODO: is_union
// TODO: is_class
// TODO: is_function
// TODO: is_reference
// TODO: is_arithmetic
// TODO: is_fundamental
// TODO: is_object
// TODO: is_scalar
// TODO: is_compound


template <class T>
struct is_member_pointer : false_type {};

template <class ClassType, class MemberType>
struct is_member_pointer<MemberType (ClassType::*)> : true_type {};


template <class T>
struct is_const : false_type {};

template <class T>
struct is_const<const T> : true_type {};


template <class T>
struct is_volatile : false_type {};

template <class T>
struct is_volatile<volatile T> : true_type {};


// TODO: is_trivial
// TODO: is_trivially_copyable
// TODO: is_standard_layout
// TODO: is_pod
// TODO: is_literal_type
// TODO: is_empty
// TODO: is_polymorphic
// TODO: is_abstract
// TODO: is_signed
// TODO: is_unsigned
// TODO: is_constructible
// ...
// TODO: extent


template <class T, class U>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};


// TODO: is_base_of
// TODO: is_convertible
// TODO: remove_reference


template <class T>
struct remove_lvalue_reference {
  typedef T type;
};

template <class T>
struct remove_lvalue_reference<T&> {
  typedef T type;
};


template <class T>
struct remove_const {
  typedef T type;
};

template <class T>
struct remove_const<const T> {
  typedef T type;
};


template <class T>
struct remove_volatile {
  typedef T type;
};

template <class T>
struct remove_volatile<volatile T> {
  typedef T type;
};


template <class T>
struct remove_cv {
  typedef typename remove_const<typename remove_volatile<T>::type>::type type;
};


template <class T>
struct add_const {
  typedef const T type;
};


template <class T>
struct add_volatile {
  typedef volatile T type;
};


template <class T>
struct add_cv {
  typedef const volatile T type;
};


template <class T>
struct add_lvalue_reference {
  typedef T& type;
};

template <class T>
struct add_lvalue_reference<T&> {
  typedef T& type;
};


// TODO: add_rvalue_reference
// TODO: make_signed
// TODO: make_unsigned


template <class T>
struct remove_extent {
  typedef T type;
};

template <class T>
struct remove_extent<T[]> {
  typedef T type;
};

template <class T, size_t N>
struct remove_extent<T[N]> {
  typedef T type;
};


template <class T>
struct remove_all_extents {
  typedef T type;
};

template <class T>
struct remove_all_extents<T[]> : remove_all_extents<T> {};

template <class T, size_t N>
struct remove_all_extents<T[N]> : remove_all_extents<T> {};


namespace detail {
template <class T>
struct remove_pointer_helper {
  typedef T type;
};

template <class T>
struct remove_pointer_helper<T*> {
  typedef T type;
};
} // namespace detail

template <class T>
struct remove_pointer {
  typedef typename detail::remove_pointer_helper<typename remove_cv<T>::type>::type type;
};


// TODO: add_pointer
// TODO: aligned_storage
// TODO: aligned_union
// TODO: decay


template <bool, class, class T>
struct conditional_c {
  typedef T type;
};

template <class T, class U>
struct conditional_c<true, T, U> {
  typedef T type;
};

template <class B, class T, class U>
struct conditional : conditional_c<B::value, T, U> {};


// TODO: common_type
// TODO: underlying_type
// TODO: result_of

}
}

#endif // #ifndef INCLUDE_GUARD_PFI_LANG_TYPE_TRAITS_H_
