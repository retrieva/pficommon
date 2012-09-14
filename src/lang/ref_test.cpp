#include "ref.h"
#include "gtest/gtest.h"

using pfi::lang::reference_wrapper;
using pfi::lang::ref;
using pfi::lang::cref;


TEST(ref, construct)
{
  int n = 2;

  {
    reference_wrapper<int> r(ref(n));
    reference_wrapper<const int> cr(cref(n));

    EXPECT_EQ(2, r);
    EXPECT_EQ(2, cr);
    EXPECT_EQ(&n, &r.get());
    EXPECT_EQ(&n, &cr.get());

    EXPECT_EQ(&n, &ref(r).get());
    EXPECT_EQ(&n, &ref(cr).get());
    EXPECT_EQ(&n, &cref(r).get());
    EXPECT_EQ(&n, &cref(cr).get());
  }

  {
    reference_wrapper<int> r(n);
    reference_wrapper<const int> cr(n);

    EXPECT_EQ(2, r);
    EXPECT_EQ(2, cr);
    EXPECT_EQ(&n, &r.get());
    EXPECT_EQ(&n, &cr.get());
  }
}

TEST(ref, assign)
{
  int n = 2;
  int dummy;

  reference_wrapper<int> r(dummy);
  r = ref(n);
  EXPECT_EQ(n, r);
  EXPECT_EQ(&n, &r.get());
  r = r;
  EXPECT_EQ(n, r);
  EXPECT_EQ(&n, &r.get());

  reference_wrapper<int> cr(dummy);
  cr = ref(n);
  EXPECT_EQ(n, cr);
  EXPECT_EQ(&n, &cr.get());
  cr = cr;
  EXPECT_EQ(n, cr);
  EXPECT_EQ(&n, &cr.get());
}

TEST(ref, ref)
{
  int n = 2;
  reference_wrapper<int> r(n);
  r = ref(n);
  r = ref(r);

  reference_wrapper<const int> cr(n);
  cr = cref(n);
  cr = ref(cr);
  cr = cref(cr);
}

TEST(ref, implicit_conversion)
{
  int n = 2;

  reference_wrapper<int> r(n);
  int& rn = r;
  EXPECT_EQ(&n, &rn);

  reference_wrapper<const int> cr(n);
  const int& crn = cr;
  EXPECT_EQ(&n, &crn);
}

TEST(ref, get)
{
  int n = 2;

  reference_wrapper<int> r(n);
  EXPECT_EQ(&n, &r.get());

  reference_wrapper<const int> cr(n);
  EXPECT_EQ(&n, &cr.get());
}

TEST(ref, get_pointer)
{
  int n = 2;

  reference_wrapper<int> r(n);
  EXPECT_EQ(&n, r.get_pointer());

  reference_wrapper<const int> cr(n);
  EXPECT_EQ(&n, cr.get_pointer());
}

namespace {
int plus1(int n) { return n + 1; }
}


TEST(ref, function_call)
{
  reference_wrapper<int (int)> f = ref(plus1);
  EXPECT_EQ(10, f(9));
}
