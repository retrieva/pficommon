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

#ifndef INCLUDE_GUARD_PFI_NETWORK_IPV4_H_
#define INCLUDE_GUARD_PFI_NETWORK_IPV4_H_

#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <string>

namespace pfi {
namespace network {

class ipv4_address {
public:
  ipv4_address() : ip() {}

  ipv4_address(unsigned char a,
               unsigned char b,
               unsigned char c,
               unsigned char d)
  {
    ip[0] = a;
    ip[1] = b;
    ip[2] = c;
    ip[3] = d;
  }

  explicit ipv4_address(const std::string& s) {
    int buf[4];
    if (std::sscanf(s.c_str(), "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]) != 4) {
      *this = none;
      return;
    }
    for (size_t i = 0; i < sizeof(buf)/sizeof(buf[0]); ++i) {
      ip[i] = buf[i];
      if (buf[i] < 0 || buf[i] > 255) {
        *this = none;
        return;
      }
    }
  }

  bool operator==(const ipv4_address& p) const {
    return std::equal(ip, ip+sizeof(ip), p.ip);
  }
  bool operator!=(const ipv4_address& p) const {
    return !(*this == p);
  }
  bool operator<(const ipv4_address& p) const {
    return std::lexicographical_compare(ip, ip+sizeof(ip), p.ip, p.ip+sizeof(p.ip));
  }

  std::string to_string() const {
    char buf[16];
    std::sprintf(buf, "%d.%d.%d.%d", int(ip[0]), int(ip[1]), int(ip[2]), int(ip[3]));
    return buf;
  }

  static const ipv4_address any;
  static const ipv4_address broadcast;
  static const ipv4_address loopback;
  static const ipv4_address none;

private:
  unsigned char ip[4];
};

} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_IPV4_H_
