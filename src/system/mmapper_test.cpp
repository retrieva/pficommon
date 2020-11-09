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

#include <gtest/gtest.h>

#include "./mmapper.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
using namespace pfi::system::mmapper;

TEST(mmapper_test, move_constructor)
{
  const std::string filename = "test.txt";
  const std::string data = "0123456789";
  const size_t size = data.size();
  {
    std::ofstream fs(filename, std::ios::out | std::ios::trunc);
    fs << data;
  }
  {
    mmapper rhs;
    EXPECT_EQ(0, rhs.open(filename));
    EXPECT_TRUE(rhs.is_open());
    EXPECT_EQ(size, rhs.size());

    mmapper lhs(std::move(rhs));
    EXPECT_TRUE(lhs.is_open());
    EXPECT_EQ(size, lhs.size());
    EXPECT_FALSE(rhs.is_open());
    EXPECT_EQ(0u, rhs.size());
  }
  {
    unlink(filename.c_str());
  }
}

TEST(mmapper_test, move_assignment_operator)
{
  const std::string filename = "test.txt";
  const std::string data = "0123456789";
  const size_t size = data.size();
  {
    std::ofstream fs(filename, std::ios::out | std::ios::trunc);
    fs << data;
  }
  {
    mmapper rhs;
    int r = rhs.open(filename);
    EXPECT_EQ(r, 0);
    EXPECT_TRUE(rhs.is_open());
    EXPECT_EQ(size, rhs.size());

    mmapper lhs = std::move(rhs);
    EXPECT_TRUE(lhs.is_open());
    EXPECT_EQ(size, lhs.size());
    EXPECT_FALSE(rhs.is_open());
    EXPECT_EQ(0u, rhs.size());
  }
  {
    unlink(filename.c_str());
  }
}

TEST(mmapper_test, move_assignment_operator_when_rhs_is_open)
{
  const std::string filename1 = "test1.txt", filename2 = "test2.txt";
  const std::string data1 = "0123456789", data2 = "01234";
  const size_t size1 = data1.size(), size2 = data2.size();
  {
    std::ofstream fs1(filename1, std::ios::out | std::ios::trunc);
    fs1 << data1;
    std::ofstream fs2(filename2, std::ios::out | std::ios::trunc);
    fs2 << data2;
  }
  {
    mmapper rhs;
    EXPECT_EQ(0, rhs.open(filename2));
    EXPECT_TRUE(rhs.is_open());
    EXPECT_EQ(size2, rhs.size());

    mmapper lhs;
    EXPECT_EQ(0, lhs.open(filename1));
    EXPECT_TRUE(lhs.is_open());
    EXPECT_EQ(size1, lhs.size());

    lhs = std::move(rhs);
    EXPECT_TRUE(lhs.is_open());
    EXPECT_EQ(size2, lhs.size());
    EXPECT_FALSE(rhs.is_open());
    EXPECT_EQ(0u, rhs.size());
  }
  {
    unlink(filename1.c_str());
    unlink(filename2.c_str());
  }
}

TEST(mmapper_test, open_close_empty)
{
  {
    ofstream ofs("test.file", std::ios::out | std::ios::trunc | std::ios::binary);
  }
  mmapper m;
  int r = m.open("test.file");
  EXPECT_EQ(r, -1);
  EXPECT_FALSE(m.is_open());
  unlink("test.file");
}

TEST(mmapper_test, open_close)
{
  {
    FILE* f;
    f = fopen("test.txt", "w+");
    EXPECT_TRUE(f != NULL);
    size_t s = fwrite("12345678", 1, 8, f);
    EXPECT_EQ(s, 8U);
    fclose(f);
  }
  {
    int r;
    mmapper m;
    r = m.open("test.txt");
    EXPECT_EQ(r, 0);
    EXPECT_TRUE(m.is_open());
    EXPECT_EQ(m.size(), 8U);
    for (int i = 0; i < int(m.size()); i++)
      EXPECT_EQ(m[i], '1' + i);
    EXPECT_EQ(m.end() - m.begin(), 8);
    EXPECT_EQ(m.cend() - m.cbegin(), 8);
    EXPECT_EQ(m.cend() - m.begin(), 8);
    for (mmapper::const_iterator it = m.cbegin(), end = m.cend(); it != end; ++it)
      EXPECT_EQ(*it, '1' + (it - m.cbegin()));
    r = m.close();
    EXPECT_EQ(r, 0);
  }
  {
    unlink("test.txt");
  }
}

TEST(mmapper_test, file_not_found)
{
  mmapper m;
  int r = m.open("file_not_found.txt");
  EXPECT_EQ(r, -1);
  EXPECT_FALSE(m.is_open());
}

TEST(mmapper_test, swap)
{
  {
    std::ofstream fs("test.txt", std::ios::out | std::ios::trunc);
    fs << "0123456789";
  }
  {
    mmapper m;
    int r = m.open("test.txt");
    EXPECT_EQ(r, 0);
    EXPECT_TRUE(m.is_open());

    mmapper m2;
    m2.swap(m);
    EXPECT_FALSE(m.is_open());
    EXPECT_TRUE(m2.is_open());
    EXPECT_EQ(m2[0], '0');

    swap(m, m2);
    EXPECT_TRUE(m.is_open());
    EXPECT_FALSE(m2.is_open());
    EXPECT_EQ(m[0], '0');
  }
  {
    unlink("test.txt");
  }
}
