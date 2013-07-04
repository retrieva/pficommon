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

#include <cstdio>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "dns.h"
#include "../system/syscall.h"

using namespace std;

using namespace pfi::lang;
using namespace pfi::concurrent;
using namespace pfi::system::syscall;

const int buf_size=8*1024;

#include <iostream>

namespace pfi{
namespace network{

pfi::lang::shared_ptr<dns_resolver> stream_socket::resolver;
r_mutex stream_socket::resolver_m;

class sigign{
public:
  sigign(){
    signal(SIGPIPE, SIG_IGN);
  }
};

stream_socket::stream_socket()
  : connected(false)
  , sock(-1)
  , raddr()
  , rport(0)
  , rdbuf(buf_size)
  , rdbuf_p(0), rdbuf_end(0)
  , wrbuf(buf_size)
  , wrbuf_size(0)
{
  static sigign ign_;
  open();
}

stream_socket::~stream_socket()
{
  flush();
  close();
}

void stream_socket::set_dns_resolver(const pfi::lang::shared_ptr<dns_resolver>& r)
{
  pfi::concurrent::scoped_lock lock(resolver_m);
  if (lock)
    resolver = r;
}

bool stream_socket::connect(const string &host, uint16_t port_num)
{
  if (sock<0) return false;

  if (connected){
    if (!close())
      return false;
    if (!open())
      return false;
  }

  pfi::lang::shared_ptr<dns_resolver> res;
  {
    pfi::concurrent::scoped_lock lock(resolver_m);
    if (lock) {
      if (!resolver)
        set_dns_resolver(pfi::lang::shared_ptr<dns_resolver>(new normal_dns_resolver()));
      res = resolver;
    }
  }
  vector<ipv4_address> ips=res->resolve(host, port_num);

  for (int i=0;i<(int)ips.size();i++){
    sockaddr_in addrin={};
    addrin.sin_family=PF_INET;
    addrin.sin_addr.s_addr=inet_addr(ips[i].to_string().c_str());
    addrin.sin_port=htons(port_num);

    int res;
    NO_INTR(res,::connect(sock,(sockaddr*)&addrin,sizeof(addrin)));
    if (SUCCEEDED(res)){
      connected=true;
      raddr=ips[i];
      rport=port_num;
      return true;
    }
  }

  (void)close();
  return false;
}

bool stream_socket::shutdown()
{
  if (!close()) return false;
  if (!open()) return false;
  return true;
}

bool stream_socket::bind_socket(int sock)
{
  return bind_socket(sock, ipv4_address(), 0);
}

bool stream_socket::bind_socket(int sock, const ipv4_address &addr, uint16_t port)
{
  if (sock<0) return false;

  if (this->sock>=0)
    if (!close())
      return false;

  this->sock=sock;
  connected=true;
  raddr=addr;
  rport=port;
  return true;
}

bool stream_socket::open()
{
  if (sock>=0) return false;

  NO_INTR(sock,::socket(PF_INET,SOCK_STREAM,0));
  if (FAILED(sock)){
    sock=-1;
    return false;
  }
  return true;
}

bool stream_socket::close()
{
  if (sock<0)
    return false;

  int ret;
  NO_INTR(ret,::close(sock));

  if (FAILED(ret)){
    sock=-1;
    connected=false;
    return false;
  }

  sock=-1;
  connected=false;
  raddr=ipv4_address();
  rport=0;
  return true;
}

bool stream_socket::is_connected() const
{
  return connected;
}

const ipv4_address &stream_socket::remote_addr() const
{
  return raddr;
}

uint16_t stream_socket::remote_port() const
{
  return rport;
}

int stream_socket::read(void *dat, int size)
{
  char *p=(char*)dat, *q=p+size;

  if (buf_elem()>0){
    if (buf_elem()>=size){
      memcpy(p, &rdbuf[rdbuf_p], size);
      rdbuf_p+=size;
      return size;
    }
    else{
      memcpy(p, &rdbuf[rdbuf_p], rdbuf_end-rdbuf_p);
      p+=rdbuf_end-rdbuf_p;
      rdbuf_p=rdbuf_end;
    }
  }

  while(p<q){
    int readsize=recv(p,q-p);
    if (readsize<=0) break;
    p+=readsize;
  }

  return p-(char*)dat;
}

int stream_socket::write(const void *dat, int size)
{
  int init_size=wrbuf_size;
  const unsigned char *st=static_cast<const unsigned char*>(dat);
  const unsigned char *p=st, *q=p+size;

  // write to buffer
  while(wrbuf_size<buf_size && p<q)
    wrbuf[wrbuf_size++]=*(p++);

  // if buffer is full, flush buffer.
  if (wrbuf_size==buf_size){
    int res=flush();
    if (res>=0){
      return max(0, res-init_size);
    }
  }

  // rest is larger than buffer, it send immediately
  if (q-p>=buf_size)
    return p-st+send_all(p, q-p);

  // otherwise, write to buffer
  while(p<q)
    wrbuf[wrbuf_size++]=*(p++);

  return q-st;
}

// if succeeded, returns negative value,
// otherwise, returns number of sent bytes
int stream_socket::flush()
{
  if (wrbuf_size>0){
    int ssize=send_all(&wrbuf[0], wrbuf_size);
    bool ok=ssize==wrbuf_size;
    wrbuf_size=0;
    return ok?-1:ssize;
  }
  return -1;
}

int stream_socket::getc()
{
  if (buf_elem()) return inc_buf();
  fill_buf();
  if (buf_elem()==0) return EOF;
  return inc_buf();
}

bool stream_socket::getline(string &str, int limit)
{
  string ret;
  for (int i=0;limit<0||i<limit;i++){
    int c=getc();
    if (c=='\n'){
      if (ret.length()>=1 && ret[ret.length()-1]=='\r'){
        str=ret.substr(0,ret.length()-1);
        return true;
      }
      break;
    }
    if (c==EOF){
      if (ret.length()==0)
        return false;
      break;
    }
    ret+=(char)c;
  }
  str=ret;
  return true;
}

bool stream_socket::puts(const string &str)
{
  int writesize=write(str.c_str(), str.length());
  return writesize==(int)str.length();
}

bool stream_socket::set_timeout(double sec)
{
  return
    set_recv_timeout(sec) &&
    set_send_timeout(sec);
}

static bool set_timeo(int sock, int optname, double sec)
{
  if (sec<0) return false;

  timeval tv;
  tv.tv_sec=max(0,(int)sec);
  tv.tv_usec=min(999999,max(0,(int)((sec-tv.tv_sec)*1e6)));

  int res;
  NO_INTR(res,setsockopt(sock,SOL_SOCKET,optname,&tv,sizeof(tv)));
  return SUCCEEDED(res);
}

bool stream_socket::set_recv_timeout(double sec)
{
  if (sock<0) return false;
  return set_timeo(sock,SO_RCVTIMEO,sec);
}

bool stream_socket::set_send_timeout(double sec)
{
  if (sock<0) return false;
  return set_timeo(sock,SO_SNDTIMEO,sec);
}

bool stream_socket::set_nodelay(bool on)
{
  if (sock<0) return false;
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

int stream_socket::recv(void *dat, int size)
{
  if (!connected) return 0;

  int readsize;
  NO_INTR(readsize, ::recv(sock, dat, size, 0));

  if (readsize<=0){
    close();
    return readsize;
  }

  return readsize;
}

int stream_socket::send(const void *dat, int size)
{
  if (!connected) return 0;

  int writesize;
  NO_INTR(writesize, ::send(sock, dat, size, 0));

  if (writesize<=0){
    close();
    return writesize;
  }

  return writesize;
}

int stream_socket::send_all(const void *dat, int size)
{
  const char *p=(const char *)dat, *q=p+size;
  
  while(p<q){
    int writesize=send(p,q-p);
    if (writesize<=0) break;
    p+=writesize;
  }
  
  return p-(const char*)dat;
}

void stream_socket::fill_buf()
{
  if (buf_elem()!=0) return;

  int size=recv(&rdbuf[0], buf_size);
  rdbuf_p=0;
  rdbuf_end=max(0, size);
}

int stream_socket::inc_buf()
{
  return (unsigned char)rdbuf[rdbuf_p++];
}

int stream_socket::buf_elem()
{
  return rdbuf_end-rdbuf_p;
}

server_socket::server_socket()
  : sock(-1)
  , port_num(~uint16_t())
  , timeout_sec(-1)
  , connected(false)
{
}

server_socket::~server_socket()
{
  close();
}

bool server_socket::create(uint16_t port_num, int backlog)
{
  int res;

  NO_INTR(sock, ::socket(PF_INET, SOCK_STREAM, 0));
  if (FAILED(sock)){
    sock=-1;
    return false;
  }

  int yes=1;
  NO_INTR(res, ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes)));

  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return false;
  }

  sockaddr_in saddr={};
  saddr.sin_family=PF_INET;
  saddr.sin_addr.s_addr=htonl(INADDR_ANY);
  saddr.sin_port=htons(port_num);

  NO_INTR(res, ::bind(sock, (sockaddr*)&saddr, sizeof(saddr)));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return false;
  }

  linger ling={0,0};
  NO_INTR(res, ::setsockopt(sock, SOL_SOCKET, SO_LINGER,
        &ling, sizeof(ling)));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return false;
  }

  NO_INTR(res, ::listen(sock, backlog));
  if (FAILED(res)){
    NO_INTR(res, ::close(sock));
    return false;
  }

  this->port_num=port_num;
  connected=true;
  return true;
}

bool server_socket::close()
{
  if (!connected)
    return false;

  int ret;
  NO_INTR(ret,::close(sock));

  if (FAILED(ret)){
    sock=-1;
    connected=false;
    return false;
  }

  sock=-1;
  connected=false;
  return true;
}

bool server_socket::is_alive() const
{
  return connected;
}

uint16_t server_socket::port() const
{
  return port_num;
}

double server_socket::timeout() const
{
  return timeout_sec;
}

bool server_socket::set_timeout(double sec)
{
  if (!connected) return false;
  return
    set_timeo(sock,SO_RCVTIMEO,sec) &&
    set_timeo(sock,SO_SNDTIMEO,sec) &&
    (timeout_sec = sec, true);
}

stream_socket *server_socket::accept()
{
  if (!connected) return NULL;

  sockaddr_in saddr={};
  socklen_t saddr_len=sizeof(saddr);

  int csock=-1;
  NO_INTR(csock, ::accept(sock, (sockaddr*)&saddr, &saddr_len));
  if (FAILED(csock))
    return NULL;

  stream_socket *ret=new stream_socket();

  char addrp[INET_ADDRSTRLEN];
  inet_ntop(saddr.sin_family, &saddr.sin_addr, addrp, sizeof(addrp));
  ret->bind_socket(csock, ipv4_address(addrp), ntohs(saddr.sin_port));

  return ret;
}

} // network
} // pfi
