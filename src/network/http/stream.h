// Copyright (c)2008-2013, Preferred Infrastructure Inc.
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

#ifndef INCLUDE_GUARD_PFI_NETWORK_HTTP_STREAM_H_
#define INCLUDE_GUARD_PFI_NETWORK_HTTP_STREAM_H_

#include "base.h"

#include <iostream>

#include "../../data/encoding/base64.h"

namespace pfi {
namespace network {
namespace http {

template <class C, class T = std::char_traits<C> >
class basic_httpstreambuf : public std::basic_streambuf<C, T> {
public:
  explicit basic_httpstreambuf(const std::string& url)
    : buf(T::eof()) 
  {
    uri u(url);
    if (u.scheme() != "http" &&
        u.scheme() != "")
      throw http::http_exception("scheme is invalid");

    int port = u.port() == "" ? 80 : pfi::lang::lexical_cast<int>(u.port());

    pfi::lang::shared_ptr<stream_socket> sock(new stream_socket());

    if (!sock->connect(u.host(), port))
      throw http_exception("cannot connect to host");

    request rq(method::get, u);
    rq.head()["Host"] = u.host();
    rq.head()["Connection"] = "close";

    if (u.userinfo() != "")
      rq.head()["Authorization"] = "Basic " + pfi::data::encoding::base64_encode(u.userinfo());

    rq.send(sock);

    resp = response(sock);
  }

  int uflow() {
    int ret = underflow();
    buf = T::eof();
    return ret;
  }

  int underflow() {
    if (buf == T::eof())
      buf = resp.body().get();
    return buf;
  }

  header& head() {
    return resp.head();
  }

private:
  int buf;
  response resp;
};

template <class C, class T = std::char_traits<C> >
class basic_httpstream : public std::basic_istream<C, T> {
public:
  explicit basic_httpstream(const std::string& url)
    : buf(url)
  {
    this->init(&buf);
  }

  header& head() {
    return buf.head();
  }

private:
  basic_httpstreambuf<C, T> buf;
};

typedef basic_httpstream<char> httpstream;

} // http
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_HTTP_STREAM_H_
