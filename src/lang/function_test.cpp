#include <stdio.h>
#include <functional>
#include <typeinfo>

#include "function.h"
#include "gtest/gtest.h"

using pfi::lang::function;


TEST(function, constructor)
{
  function<void ()> f;
  function<void (int**** const**** const** const&, function<void ()>)> f2;

  function<int (const char*)> f3(printf);
  function<void (const char*)> f4(printf);

  function<int (const char*)> f5(f3);
  function<void (const char*)> f6(f5);
}

TEST(function, assignment)
{
  function<int (int, int)> f;
  f = f;
  f = std::plus<int>();
  EXPECT_EQ(3, f(1, 2));
}

TEST(function, swap)
{
  function<int (int, int)> f;
  function<int (int, int)> f2 = std::minus<int>();
  f.swap(f2);
  EXPECT_EQ(3, f(5, 2));
  EXPECT_THROW(f2(5, 2);, std::exception);

  swap(f, f2);
  EXPECT_THROW(f(10, 5);, std::exception);
  EXPECT_EQ(5, f2(10, 5));
}

TEST(function, target)
{
  function<int (int, int)> f = std::multiplies<int>();
  const std::type_info& ti = f.target_type();
  EXPECT_EQ(typeid(std::multiplies<int>), ti);

  std::multiplies<int>* t = f.target<std::multiplies<int> >();
  EXPECT_EQ(15, (*t)(3, 5));

  const std::multiplies<int>* ct = const_cast<const function<int (int, int)>&>(f).target<std::multiplies<int> >();
  EXPECT_EQ(1, (*ct)(-1, -1));
}

TEST(function, comparison_with_null_pointer)
{
  function <int (const char*)> f;
  EXPECT_TRUE(f == 0);
  EXPECT_TRUE(0 == f);
  EXPECT_FALSE(f != 0);
  EXPECT_FALSE(0 != f);

  f = printf;
  EXPECT_FALSE(f == 0);
  EXPECT_FALSE(0 == f);
  EXPECT_TRUE(f != 0);
  EXPECT_TRUE(0 != f);
}
