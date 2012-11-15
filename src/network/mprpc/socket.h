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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_SOCKET_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_SOCKET_H_

#include <vector>

#include "../../concurrent/mutex.h"
#include "../../network/dns.h"
#include "../../lang/shared_ptr.h"
#include "../../lang/noncopyable.h"

namespace pfi {
namespace network {
namespace mprpc {


class socket : pfi::lang::noncopyable {
public:
  socket();
  socket(int fd);
  ~socket();

  bool close();

  int get() { return fd; }
  int release() { int tmp = fd; fd = -1; return tmp; }

  bool connect(const std::string& host, uint16_t port);
  bool listen(uint16_t port, int backlog=4096);

  bool set_nodelay(bool on);
  bool set_timeout(double sec);
  bool set_recv_timeout(double sec);
  bool set_send_timeout(double sec);

  static int connect_sock(const std::string& host, uint16_t port);
  static int listen_sock(uint16_t port, int backlog=4096);

  static bool set_nodelay_sock(int sock, bool on);
  static bool set_timeout_sock(int sock, double sec);
  static bool set_recv_timeout_sock(int sock, double sec);
  static bool set_send_timeout_sock(int sock, double sec);

  static std::vector<ipv4_address> resolve(const std::string& host, uint16_t port);

private:
  int fd;

  static bool set_timeout_sockopt(int sock, int optname, double sec);

  static pfi::lang::shared_ptr<dns_resolver> resolver;
  static pfi::concurrent::r_mutex resolver_m;
};


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_SOCKET_H_
