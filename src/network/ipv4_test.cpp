// Copyright (c)2008-2012, Preferred Infrastructure Inc.
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

#include "ipv4.h"

using pfi::network::ipv4_address;

TEST(ipv4_address, construct_none)
{
  ipv4_address ip;
  ipv4_address ip0000(0, 0, 0, 0);
  EXPECT_EQ(ip0000, ip);
}

TEST(ipv4_address, construct_from_string)
{
  ipv4_address ip_from_ints(128, 128, 128, 128);
  ipv4_address ip_from_string("128.128.128.128");
  EXPECT_EQ(ip_from_ints, ip_from_string);
}

TEST(ipv4_address, compare_to_self)
{
  ipv4_address ip;
  EXPECT_TRUE(ip == ip);
  EXPECT_FALSE(ip != ip);
  EXPECT_FALSE(ip < ip);
}

TEST(ipv4_address, compare_pos_3)
{
  ipv4_address ip1(1, 1, 1, 1);
  ipv4_address ip2(1, 1, 1, 2);
  EXPECT_FALSE(ip1 == ip2);
  EXPECT_TRUE(ip1 != ip2);
  EXPECT_TRUE(ip1 < ip2);
  EXPECT_FALSE(ip2 < ip1);
}

TEST(ipv4_address, compare_pos_2)
{
  ipv4_address ip1(1, 1, 1, 1);
  ipv4_address ip2(1, 1, 2, 0);
  EXPECT_FALSE(ip1 == ip2);
  EXPECT_TRUE(ip1 != ip2);
  EXPECT_TRUE(ip1 < ip2);
  EXPECT_FALSE(ip2 < ip1);
}

TEST(ipv4_address, compare_pos_1)
{
  ipv4_address ip1(1, 1, 1, 1);
  ipv4_address ip2(1, 2, 0, 1);
  EXPECT_FALSE(ip1 == ip2);
  EXPECT_TRUE(ip1 != ip2);
  EXPECT_TRUE(ip1 < ip2);
  EXPECT_FALSE(ip2 < ip1);
}

TEST(ipv4_address, compare_pos_0)
{
  ipv4_address ip1(1, 1, 1, 1);
  ipv4_address ip2(2, 0, 1, 1);
  EXPECT_FALSE(ip1 == ip2);
  EXPECT_TRUE(ip1 != ip2);
  EXPECT_TRUE(ip1 < ip2);
  EXPECT_FALSE(ip2 < ip1);
}

TEST(ipv4_address, to_string_none)
{
  ipv4_address ip;
  EXPECT_EQ("0.0.0.0", ip.to_string());
}

TEST(ipv4_address, to_string)
{
  ipv4_address ip(123, 123, 123, 123);
  EXPECT_EQ("123.123.123.123", ip.to_string());
}

class ipv4_invalid : public testing::TestWithParam<std::string> {
};

INSTANTIATE_TEST_CASE_P(ipv4_invalid_instance,
                        ipv4_invalid,
                        testing::Values("192", "192.", "192.168", "192.168.", "192.168.0", "192.168.0.",
                                        "1192.168.0.1", "192.1168.0.1", "192.168.1000.1", "192.168.0.1111",
                                        "256.0.0.0", "0.256.0.0", "0.0.256.0", "0.0.0.256",
                                        "-1.0.0.0", "0.-1.0.0", "0.0.-1.0", "0.0.0.-1",
                                        "192.168,0.1"));

// TODO: "192.168.0.1." should be invalid
// TODO: tests for octal numbers

TEST_P(ipv4_invalid, test)
{
  EXPECT_EQ(ipv4_address::none, ipv4_address(GetParam()));
}
