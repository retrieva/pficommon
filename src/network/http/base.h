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

#ifndef INCLUDE_GUARD_PFI_NETWORK_HTTP_BASE_H_
#define INCLUDE_GUARD_PFI_NETWORK_HTTP_BASE_H_

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>

#include "../uri.h"
#include "../../lang/shared_ptr.h"
#include "../../lang/function.h"
#include "../../network/socket.h"

namespace pfi {
namespace network {
namespace http {

class http_exception : public std::runtime_error {
public:
  explicit http_exception(const std::string& reason) : runtime_error(reason) {}
};

class header {
public:
  typedef std::vector<std::pair<std::string, std::string> > dat_type;
  typedef dat_type::iterator iterator;
  typedef dat_type::const_iterator const_iterator;

  header();
  explicit header(const pfi::lang::shared_ptr<stream_socket>& sock);
  explicit header(std::istream& is);

  ~header();

  bool has_key(const std::string &key) const;

  const std::string& get(const std::string& key);
  void set(const std::string& key, const std::string& val);
  void erase(const std::string& key);

  std::string& operator[](const std::string& key);
  const std::string& operator[](const std::string& key) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  void send(const pfi::lang::shared_ptr<stream_socket>& sock);

private:
  void read_header(pfi::lang::function<bool (std::string*)> f);

  std::vector<std::pair<std::string,std::string> > dat;
};

class method {
public:
  static const std::string get;
  static const std::string post;
  static const std::string head;
  static const std::string put;
  static const std::string del; // delete
  static const std::string trace;
  static const std::string connect;
  static const std::string options;
};

class request {
public:
  request(const std::string& method, const uri& u, int major_ver = 1, int minor_ver = 1);
  explicit request(const pfi::lang::shared_ptr<stream_socket>& sock);

  ~request();

  const std::string& method() const;
  const pfi::network::uri& path() const;
  const std::pair<int,int>& version() const;

  header& head();
  std::iostream& body();

  // Send a request to socket
  // Don't call this method twice to same object.
  void send(const pfi::lang::shared_ptr<stream_socket>& sock);

private:
  std::string method_;
  pfi::network::uri uri_;
  std::pair<int,int> version_;

  header header_;
  pfi::lang::shared_ptr<std::iostream> stream;
};

class response {
public:
  response();
  response(int code, const std::string& reason, int major_ver = 1, int minor_ver = 1);
  explicit response(const pfi::lang::shared_ptr<stream_socket>& sock);

  ~response();

  const std::pair<int, int>& version() const;
  int code() const;
  const std::string& reason() const;

  header& head();
  std::iostream& body();

  // Send a response to socket
  // Don't call this method twice to same object.
  void send(const pfi::lang::shared_ptr<stream_socket>& sock);

private:
  std::pair<int, int> version_;
  int code_;
  std::string reason_;

  header header_;
  pfi::lang::shared_ptr<std::iostream> stream;
};

} // http
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_HTTP_BASE_H_
