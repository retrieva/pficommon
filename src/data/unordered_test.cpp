#include <gtest/gtest.h>

#include "./unordered_set.h"
#include "./unordered_map.h"

TEST(unordered_test, reserve) {
  std::size_t n = 10000;

  {
    pfi::data::unordered_set<int> us;
    us.reserve(n);
    EXPECT_GE(us.bucket_count(), std::ceil(n / us.max_load_factor()));
  }

  {
    pfi::data::unordered_multiset<int> ums;
    ums.reserve(n);
    EXPECT_GE(ums.bucket_count(), std::ceil(n / ums.max_load_factor()));
  }

  {
    pfi::data::unordered_map<int, int> um;
    um.reserve(n);
    EXPECT_GE(um.bucket_count(), std::ceil(n / um.max_load_factor()));
  }

  {
    pfi::data::unordered_multimap<int, int> umm;
    umm.reserve(n);
    EXPECT_GE(umm.bucket_count(), std::ceil(n / umm.max_load_factor()));
  }
}
