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

#include "socket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "../../system/syscall.h"

namespace pfi {
namespace network {
namespace mprpc {

pfi::lang::shared_ptr<dns_resolver> socket::resolver;
pfi::concurrent::r_mutex socket::resolver_m;


socket::socket() : fd(-1) { }
socket::socket(int fd) : fd(fd) { }

socket::~socket()
{
  close();
}

bool socket::close()
{
  if(fd < 0) { return false; }

  int ret;
  ::shutdown(fd, SHUT_RDWR);
  NO_INTR(ret,::close(fd));

  if (FAILED(ret)){
    fd = -1;
    return false;
  }

  fd = -1;
  return true;
}


bool socket::connect(const std::string& host, uint16_t port)
{
  int nfd = connect_sock(host, port);
  if(nfd < 0) {
    return false;
  }
  close();
  fd = nfd;
  return true;
}

bool socket::listen(uint16_t port, int backlog)
{
  int nfd = listen_sock(port, backlog);
  if(nfd < 0) {
    return false;
  }
  close();
  fd = nfd;
  return true;
}

std::vector<ipv4_address> socket::resolve(const std::string& host, uint16_t port)
{
  pfi::lang::shared_ptr<dns_resolver> res;
  {
    pfi::concurrent::scoped_lock lock(resolver_m);
    if (lock) {
      if (!resolver) {
        resolver.reset( new normal_dns_resolver() );
      }
      res = resolver;
    }
  }
  return res->resolve(host, port);
}

int socket::connect_sock(const std::string& host, uint16_t port)
{
  int sock;
  NO_INTR(sock, ::socket(PF_INET, SOCK_STREAM, 0));
  if (FAILED(sock)){
    return -1;
  }

  int res;

  std::vector<ipv4_address> ips = resolve(host, port);
  for (int i=0; i < (int)ips.size(); i++){
    sockaddr_in addrin={};
    addrin.sin_family = PF_INET;
    addrin.sin_addr.s_addr = inet_addr(ips[i].to_string().c_str());
    addrin.sin_port = htons(port);

    NO_INTR(res,::connect(sock,(sockaddr*)&addrin,sizeof(addrin)));
    if (SUCCEEDED(res)) {
      return sock;
    }
  }

  NO_INTR(res, ::close(sock));
  return -1;
}

int socket::listen_sock(uint16_t port, int backlog)
{
  int res;
  int sock;

  NO_INTR(sock, ::socket(PF_INET, SOCK_STREAM, 0));
  if (FAILED(sock)) {
    return -1;
  }

  int yes = 1;
  NO_INTR(res, ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes)));

  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return -1;
  }

  sockaddr_in saddr = {};
  saddr.sin_family = PF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);

  NO_INTR(res, ::bind(sock, (sockaddr*)&saddr, sizeof(saddr)));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return -1;
  }

  linger ling={0,0};
  NO_INTR(res, ::setsockopt(sock, SOL_SOCKET, SO_LINGER,
        &ling, sizeof(ling)));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return -1;
  }

  NO_INTR(res, ::listen(sock, backlog));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return -1;
  }

  return sock;
}


bool socket::set_nodelay(bool on)
{
  return set_nodelay_sock(fd, on);
}

bool socket::set_nodelay_sock(int sock, bool on)
{
  if(sock < 0) { return false; }
#ifdef __linux__
  int n=on?1:0;
  int res;
  NO_INTR(res,setsockopt(sock,SOL_TCP,TCP_NODELAY,&n,sizeof(n)));
  if (FAILED(res)) return false;
  NO_INTR(res,setsockopt(sock,SOL_SOCKET,TCP_DEFER_ACCEPT,&n,sizeof(n)));
  return SUCCEEDED(res);
#else
  return true;
#endif
}


bool socket::set_timeout(double sec)
{
  return set_timeout_sock(fd, sec);
}

bool socket::set_recv_timeout(double sec)
{
  return set_recv_timeout_sock(fd, sec);
}

bool socket::set_send_timeout(double sec)
{
  return set_send_timeout_sock(fd, sec);
}

bool socket::set_timeout_sock(int sock, double sec)
{
  return set_recv_timeout_sock(sock, sec) &&
    set_send_timeout_sock(sock, sec);
}

bool socket::set_recv_timeout_sock(int sock, double sec)
{
  return set_timeout_sockopt(sock, SO_RCVTIMEO, sec);
}

bool socket::set_send_timeout_sock(int sock, double sec)
{
  return set_timeout_sockopt(sock, SO_SNDTIMEO, sec);
}

bool socket::set_timeout_sockopt(int sock, int optname, double sec)
{
  if(sock < 0) { return false; }

  timeval tv;
  tv.tv_sec = std::max(0,(int)sec);
  tv.tv_usec = std::min(999999, std::max(0,(int)((sec-tv.tv_sec)*1e6)));

  int res;
  NO_INTR(res,setsockopt(sock,SOL_SOCKET,optname,&tv,sizeof(tv)));
  return SUCCEEDED(res);
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

