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

#include "type_traits.h"
#include <gtest/gtest.h>

namespace type_traits_test_detail {
struct base {};
struct derived : base {};
}
using namespace type_traits_test_detail;

TEST(type_traits, is_same)
{
  using pfi::lang::is_same;
  EXPECT_TRUE((is_same<int, int>::value));
  EXPECT_FALSE((is_same<int, long>::value));
  EXPECT_FALSE((is_same<int, int*>::value));
  EXPECT_FALSE((is_same<int, const int>::value));
  EXPECT_FALSE((is_same<base, derived>::value));
}

TEST(type_traits, integral_constant)
{
  using pfi::lang::is_same;
  using pfi::lang::integral_constant;

  typedef integral_constant<int, 3> int3;
  EXPECT_EQ(3, int3::value);
  EXPECT_TRUE((is_same<int, int3::value_type>::value));
  EXPECT_TRUE((is_same<int3, int3::type>::value));
  EXPECT_EQ(3, int3());

  using pfi::lang::false_type;
  EXPECT_FALSE(false_type::value);
  EXPECT_TRUE((is_same<bool, false_type::value_type>::value));
  EXPECT_TRUE((is_same<false_type, false_type::type>::value));
  EXPECT_FALSE(false_type());

  using pfi::lang::true_type;
  EXPECT_TRUE(true_type::value);
  EXPECT_TRUE((is_same<bool, true_type::value_type>::value));
  EXPECT_TRUE((is_same<true_type, true_type::type>::value));
  EXPECT_TRUE(true_type());
}

TEST(type_traits, is_void)
{
  using pfi::lang::is_void;

  EXPECT_TRUE(is_void<void>::value);
  EXPECT_TRUE(is_void<const void>::value);
  EXPECT_TRUE(is_void<volatile void>::value);
  EXPECT_TRUE(is_void<const volatile void>::value);

  EXPECT_FALSE(is_void<int>::value);
  EXPECT_FALSE(is_void<const int>::value);
  EXPECT_FALSE(is_void<volatile int>::value);
  EXPECT_FALSE(is_void<const volatile int>::value);
}

TEST(type_traits, is_array)
{
  using pfi::lang::is_array;

  EXPECT_FALSE(is_array<int>::value);
  EXPECT_TRUE(is_array<int[]>::value);
  EXPECT_TRUE(is_array<int[1000]>::value);
  EXPECT_TRUE(is_array<int[1][2]>::value);
}

TEST(type_traits, is_pointer)
{
  using pfi::lang::is_pointer;

  EXPECT_FALSE(is_pointer<int>::value);
  EXPECT_TRUE(is_pointer<int*>::value);
  EXPECT_TRUE(is_pointer<int* const>::value);
  EXPECT_TRUE(is_pointer<int* volatile>::value);
  EXPECT_TRUE(is_pointer<int* const volatile>::value);
}

TEST(type_traits, is_lvalue_reference)
{
  using pfi::lang::is_lvalue_reference;

  EXPECT_FALSE(is_lvalue_reference<int>::value);
  EXPECT_FALSE(is_lvalue_reference<int*>::value);
  EXPECT_TRUE(is_lvalue_reference<int&>::value);
  EXPECT_TRUE(is_lvalue_reference<const int&>::value);
}

namespace type_traits_test_detail {
struct foo;
}

TEST(type_traits, is_member_pointer)
{
  using pfi::lang::is_member_pointer;

  EXPECT_FALSE(is_member_pointer<int>::value);
  EXPECT_FALSE(is_member_pointer<int*>::value);
  EXPECT_TRUE(is_member_pointer<int (foo::*)>::value);
  typedef void function_type();
  EXPECT_TRUE(is_member_pointer<function_type (foo::*)>::value);
}

TEST(type_traits, is_const)
{
  using pfi::lang::is_const;

  EXPECT_FALSE(is_const<int>::value);
  EXPECT_TRUE(is_const<const int>::value);

  EXPECT_FALSE(is_const<const int*>::value);
  EXPECT_TRUE(is_const<int* const>::value);
}

TEST(type_traits, is_volatile)
{
  using pfi::lang::is_volatile;

  EXPECT_FALSE(is_volatile<int>::value);
  EXPECT_TRUE(is_volatile<volatile int>::value);

  EXPECT_FALSE(is_volatile<volatile int*>::value);
  EXPECT_TRUE(is_volatile<int* volatile>::value);
}

TEST(type_traits, remove_lvalue_reference)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_lvalue_reference;
  EXPECT_TRUE((is_same<int, remove_lvalue_reference<int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_lvalue_reference<int&>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_lvalue_reference<const int&>::type>::value));
}

TEST(type_traits, remove_const)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_const;

  EXPECT_TRUE((is_same<int, remove_const<int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_const<const int>::type>::value));
  EXPECT_TRUE((is_same<volatile int, remove_const<volatile int>::type>::value));
  EXPECT_TRUE((is_same<volatile int, remove_const<const volatile int>::type>::value));

  EXPECT_TRUE((is_same<const int*, remove_const<const int*>::type>::value));
  EXPECT_TRUE((is_same<const int*, remove_const<const int* const>::type>::value));
}

TEST(type_traits, remove_volatile)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_volatile;

  EXPECT_TRUE((is_same<int, remove_volatile<int>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_volatile<const int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_volatile<volatile int>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_volatile<const volatile int>::type>::value));

  EXPECT_TRUE((is_same<volatile int*, remove_volatile<volatile int*>::type>::value));
  EXPECT_TRUE((is_same<volatile int*, remove_volatile<volatile int* volatile>::type>::value));
}

TEST(type_traits, remove_cv)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_cv;

  EXPECT_TRUE((is_same<int, remove_cv<int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_cv<const int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_cv<volatile int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_cv<const volatile int>::type>::value));

  EXPECT_TRUE((is_same<const volatile int*, remove_cv<const volatile int*>::type>::value));
  EXPECT_TRUE((is_same<const volatile int*, remove_cv<const volatile int* const>::type>::value));
  EXPECT_TRUE((is_same<const volatile int*, remove_cv<const volatile int* volatile>::type>::value));
  EXPECT_TRUE((is_same<const volatile int*, remove_cv<const volatile int* const volatile>::type>::value));
}

TEST(type_traits, add_const)
{
  using pfi::lang::is_same;
  using pfi::lang::add_const;

  EXPECT_TRUE((is_same<const int, add_const<int>::type>::value));
  EXPECT_TRUE((is_same<int* const, add_const<int*>::type>::value));
}

TEST(type_traits, add_volatile)
{
  using pfi::lang::is_same;
  using pfi::lang::add_volatile;

  EXPECT_TRUE((is_same<volatile int, add_volatile<int>::type>::value));
  EXPECT_TRUE((is_same<int* volatile, add_volatile<int*>::type>::value));
}

TEST(type_traits, add_cv)
{
  using pfi::lang::is_same;
  using pfi::lang::add_cv;

  EXPECT_TRUE((is_same<const volatile int, add_cv<int>::type>::value));
  EXPECT_TRUE((is_same<int* const volatile, add_cv<int*>::type>::value));
}

TEST(type_traits, add_lvalue_reference)
{
  using pfi::lang::is_same;
  using pfi::lang::add_lvalue_reference;

  EXPECT_TRUE((is_same<int&, add_lvalue_reference<int&>::type>::value));
}

TEST(type_traits, remove_extent)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_extent;

  EXPECT_TRUE((is_same<int, remove_extent<int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_extent<int[]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_extent<int[100]>::type>::value));
}

TEST(type_traits, remove_all_extents)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_all_extents;

  EXPECT_TRUE((is_same<int, remove_all_extents<int>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[1]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[][2]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[2][2]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[][3][3]>::type>::value));
  EXPECT_TRUE((is_same<int, remove_all_extents<int[3][3][3]>::type>::value));
}

TEST(type_traits, remove_pointer)
{
  using pfi::lang::is_same;
  using pfi::lang::remove_pointer;

  EXPECT_TRUE((is_same<int, remove_pointer<int*>::type>::value));
  EXPECT_TRUE((is_same<int, remove_pointer<int* const>::type>::value));
  EXPECT_TRUE((is_same<int, remove_pointer<int* volatile>::type>::value));
  EXPECT_TRUE((is_same<int, remove_pointer<int* const volatile>::type>::value));

  EXPECT_TRUE((is_same<const int, remove_pointer<const int*>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_pointer<const int* const>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_pointer<const int* volatile>::type>::value));
  EXPECT_TRUE((is_same<const int, remove_pointer<const int* const volatile>::type>::value));

  EXPECT_TRUE((is_same<int*, remove_pointer<int**>::type>::value));
  EXPECT_TRUE((is_same<int*, remove_pointer<int** const>::type>::value));
  EXPECT_TRUE((is_same<int*, remove_pointer<int** volatile>::type>::value));
  EXPECT_TRUE((is_same<int*, remove_pointer<int** const volatile>::type>::value));

  EXPECT_TRUE((is_same<int* const, remove_pointer<int* const*>::type>::value));
  EXPECT_TRUE((is_same<int* const, remove_pointer<int* const* const>::type>::value));
  EXPECT_TRUE((is_same<int* const, remove_pointer<int* const* volatile>::type>::value));
  EXPECT_TRUE((is_same<int* const, remove_pointer<int* const* const volatile>::type>::value));
}

TEST(type_traits, conditional)
{
  using pfi::lang::is_same;
  using pfi::lang::conditional_c;
  EXPECT_TRUE((is_same<int, conditional_c<true, int, void>::type>::value));
  EXPECT_TRUE((is_same<void, conditional_c<false, int, void>::type>::value));

  using pfi::lang::true_type;
  using pfi::lang::false_type;
  using pfi::lang::conditional;
  EXPECT_TRUE((is_same<int, conditional<true_type, int, void>::type>::value));
  EXPECT_TRUE((is_same<void, conditional<false_type, int, void>::type>::value));
}
