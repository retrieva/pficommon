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

#ifndef INCLUDE_GUARD_PFI_NETWORK_SOCKET_H_
#define INCLUDE_GUARD_PFI_NETWORK_SOCKET_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <stdint.h>

#include "ipv4.h"
#include "../lang/shared_ptr.h"
#include "../concurrent/mutex.h"

namespace pfi{
namespace network{

class socket_exception : public std::exception{
};

class dns_resolver;
class server_socket;

class stream_socket{
  friend class server_socket;
public:
  stream_socket();

  ~stream_socket();

  bool connect(const std::string &host, uint16_t port_num);
  bool shutdown();

  bool is_connected() const;

  const ipv4_address &remote_addr() const;
  uint16_t remote_port() const;

  int read(void *dat, int size);
  int write(const void *dat, int size);
  int flush();

  int getc();
  bool getline(std::string &str, int limit=-1);
  bool puts(const std::string &str);

  bool set_timeout(double sec);
  bool set_recv_timeout(double sec);
  bool set_send_timeout(double sec);

  bool set_nodelay(bool on);
  // bool set_nodelay();
  // bool set_...

  static void set_dns_resolver
  (const pfi::lang::shared_ptr<dns_resolver>& resolver);

private:
  bool open();
  bool close();

  int recv(void *dat, int size);
  int send(const void *dat, int size);
  int send_all(const void *dat, int size);

  bool bind_socket(int sock);
  bool bind_socket(int sock, const ipv4_address &addr, uint16_t port);

  bool connected;
  int sock;

  ipv4_address raddr;
  uint16_t rport;

  void fill_buf();
  int inc_buf();
  int buf_elem();

  std::vector<unsigned char> rdbuf;
  int rdbuf_p, rdbuf_end;

  std::vector<unsigned char> wrbuf;
  int wrbuf_size;

  static pfi::lang::shared_ptr<dns_resolver> resolver;
  static pfi::concurrent::r_mutex resolver_m;
};

class server_socket{
public:
  server_socket();
  ~server_socket();

  bool create(uint16_t port_num, int backlog=4096);
  bool close();

  bool is_alive() const;

  uint16_t port() const;
  double timeout() const;

  bool set_timeout(double sec);

  stream_socket *accept();

private:
  int sock;
  uint16_t port_num;
  double timeout_sec;
  bool connected;
};

} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_SOCKET_H_
