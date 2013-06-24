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
  EXPECT_EQ("0123456789", net::uri_encode("0123456789"));
  EXPECT_EQ("-_.~", net::uri_encode("-_.~"));

  const std::string abc_expected = "A%3AB%2FC";
  const std::string abc_actual = net::uri_encode("A:B/C");
  EXPECT_TRUE(equal_percent_encoded_string(abc_expected, abc_actual));

  const std::string pfi_expected = "%E3%83%94%E3%83%BCFI";
  const std::string pfi_actual = net::uri_encode("ピーFI");
  EXPECT_TRUE(equal_percent_encoded_string(pfi_expected, pfi_actual));
}

TEST(uri, decode)
{
  EXPECT_EQ("", net::uri_decode(""));

  EXPECT_EQ("abcdefghijklmnopqrstuvwxyz", net::uri_decode("abcdefghijklmnopqrstuvwxyz"));
  EXPECT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", net::uri_decode("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  EXPECT_EQ("0123456789", net::uri_decode("0123456789"));
  EXPECT_EQ("-_.~", net::uri_decode("-_.~"));

  const std::string abc_expected = "A:B/C";
  const std::string abc_actual = net::uri_decode("A%3aB%2FC");
  EXPECT_EQ(abc_expected, abc_actual);

  const std::string pfi_expected = "ピーFI";
  const std::string pfi_actual = net::uri_decode("%E3%83%94%E3%83%BcFI");
  EXPECT_TRUE(equal_percent_encoded_string(pfi_expected, pfi_actual));
}

TEST(uri, class)
{
  using pfi::network::uri;

  uri pfi("http://preferred.jp/product/sedue/");
  EXPECT_EQ("http", pfi.scheme());
  EXPECT_EQ("preferred.jp", pfi.authority());
  EXPECT_TRUE(pfi.userinfo().empty());
  EXPECT_EQ("preferred.jp", pfi.host());
  EXPECT_TRUE(pfi.port().empty());
  EXPECT_EQ("/product/sedue/", pfi.path());
  EXPECT_TRUE(pfi.query().empty());
  EXPECT_TRUE(pfi.fragment().empty());

  uri complex("http://user:pass@a.com:80/aoeu/htns?q=1234&r=5678#n42");
  EXPECT_EQ("http", complex.scheme());
  EXPECT_EQ("user:pass@a.com:80", complex.authority());
  EXPECT_EQ("user:pass", complex.userinfo());
  EXPECT_EQ("a.com", complex.host());
  EXPECT_EQ("80", complex.port());
  EXPECT_EQ("/aoeu/htns", complex.path());
  EXPECT_EQ("q=1234&r=5678", complex.query());
  EXPECT_EQ("n42", complex.fragment());

  uri ipv6("http://[2001:0DB8:0000:CD30:0123:4567:89AB:CDEF]/");
  EXPECT_EQ("http", ipv6.scheme());
  EXPECT_EQ("[2001:0DB8:0000:CD30:0123:4567:89AB:CDEF]", ipv6.host());
}

TEST(uri, authority)
{
  using pfi::network::uri;

  {
    uri no_userinfo_no_port("http://www.google.com/");
    EXPECT_EQ("www.google.com", no_userinfo_no_port.authority());
  }
  {
    uri no_userinfo_port("http://www.google.com:8080/");
    EXPECT_EQ("www.google.com:8080", no_userinfo_port.authority());
  }
  {
    uri userinfo_no_port("http://user:pass@www.google.com/");
    EXPECT_EQ("user:pass@www.google.com", userinfo_no_port.authority());
  }
  {
    uri userinfo_port("http://user:pass@www.google.com:8080/");
    EXPECT_EQ("user:pass@www.google.com:8080", userinfo_port.authority());
  }
}

#include <sstream>
#include <string>

TEST(uri, output_operator)
{
  std::string uri_string("http://u:p@example.com:80/a/b/c/?p=123&q=456#n");
  pfi::network::uri uri(uri_string);

  std::ostringstream oss;
  oss << uri;

  EXPECT_EQ(uri_string, oss.str());
}
