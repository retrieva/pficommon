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
}
