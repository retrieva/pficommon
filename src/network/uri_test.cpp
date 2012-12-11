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

#include "uri.h"

#include <cctype>
#include <algorithm>
#include <string>

namespace net = pfi::network;

namespace {
bool equal_percent_encoded_string(const std::string& x, const std::string& y)
{
  typedef std::string::size_type size_type;
  const size_type size = x.size();
  if (size != y.size())
      return false;

  for (size_type i = 0; i < size; ) {
    if (x[i] != y[i])
      return false;
    if (x[i] == '%' && i+2 < size) {
      if (std::toupper(x[i+1]) != std::toupper(y[i+1]))
        return false;
      if (std::toupper(x[i+2]) != std::toupper(y[i+2]))
        return false;
      i += 3;
    } else
      ++i;
  }
  return true;
}
bool case_insensitive_equal(char x, char y)
{
  return std::toupper(x) == std::toupper(y);
}
}

TEST(uri, encode)
{
  EXPECT_EQ("", net::uri_encode(""));

  EXPECT_EQ("abcdefghijklmnopqrstuvwxyz", net::uri_encode("abcdefghijklmnopqrstuvwxyz"));
  EXPECT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", net::uri_encode("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  EXPECT_EQ("-_.~", net::uri_encode("-_.~"));

  const std::string abc_expected = "A%3AB%2FC";
  const std::string abc_actual = net::uri_encode("A:B/C");
  EXPECT_TRUE(equal_percent_encoded_string(abc_expected, abc_actual));

  const std::string pfi_expected = "%E3%83%94%E3%83%BCFI";
  const std::string pfi_actual = net::uri_encode("ピーFI");
  EXPECT_TRUE(equal_percent_encoded_string(pfi_expected, pfi_actual));
}
