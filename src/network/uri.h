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

#ifndef INCLUDE_GUARD_PFI_NETWORK_URI_H_
#define INCLUDE_GUARD_PFI_NETWORK_URI_H_

#include <string>
#include <iostream>

namespace pfi{
namespace network{

std::string uri_encode(const std::string &s);
std::string uri_decode(const std::string &s);

class uri{
public:
  explicit uri(const std::string &s);
  ~uri();

  const std::string &scheme() const;
  std::string authority() const;
  const std::string &userinfo() const;
  const std::string &host() const;
  const std::string &port() const;
  const std::string &path() const;
  const std::string &query() const;
  const std::string &fragment() const;

private:
  void clear();

  bool parse(const std::string &s);

  bool parse_uri(const char *&p);
  bool parse_relative_ref(const char *&p);
  bool parse_relative_part(const char *&p);
  bool parse_scheme(const char *&p);
  bool parse_hier_part(const char *&p);
  bool parse_authority(const char *&p);
  bool parse_userinfo(const char *&p);
  bool parse_host(const char *&p);
  bool parse_port(const char *&p);
  bool parse_ip_literal(const char *&p);
  bool parse_ipvfuture(const char *&p);
  bool parse_ipv6address(const char *&p);
  bool parse_ipv6_1(const char *&p);
  bool parse_ipv6_2(const char *&p);
  bool parse_ipv6_3(const char *&p);
  bool parse_ipv6_4(const char *&p);
  bool parse_ipv6_5(const char *&p);
  bool parse_ipv6_6(const char *&p);
  bool parse_ipv6_7(const char *&p);
  bool parse_ipv6_8(const char *&p);
  bool parse_ipv6_9(const char *&p);
  bool parse_h16(const char *&p);
  bool parse_ls32(const char *&p);
  bool parse_ipv4address(const char *&p);
  bool parse_dec_octet(const char *&p);
  bool parse_reg_name(const char *&p);
  bool parse_path_abempty(const char *&p);
  bool parse_path_absolute(const char *&p);
  bool parse_path_noscheme(const char *&p);
  bool parse_path_rootless(const char *&p);
  bool parse_path_empty(const char *&p);
  bool parse_segment(const char *&p);
  bool parse_segment_nz(const char *&p);
  bool parse_segment_nz_nc(const char *&p);
  bool parse_pchar(const char *&p, bool colon=true);
  bool parse_query(const char *&p);
  bool parse_fragment(const char *&p);
  bool parse_pct_encoded(const char *&p);
  bool is_unreserved(char c);
  bool is_reserved(char c);
  bool is_gen_delims(char c);
  bool is_sub_delims(char c);

  std::string scheme_;
  std::string userinfo_;
  std::string host_;
  std::string port_;
  std::string path_;
  std::string query_;
  std::string fragment_;
};

std::ostream &operator<<(std::ostream &os, const uri &u);

} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_URI_H_
