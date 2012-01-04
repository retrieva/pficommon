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

#ifndef INCLUDE_GUARD_PFI_NETWORK_IOSTREAM_H_
#define INCLUDE_GUARD_PFI_NETWORK_IOSTREAM_H_

#include <streambuf>
#include <stdexcept>
#include <stdint.h>

#include "socket.h"
#include "../lang/shared_ptr.h"

namespace pfi{
namespace network{

template <class C, class T = std::char_traits<C> >
class basic_socketstreambuf : public std::basic_streambuf<C,T>{
public:
  typedef C char_type;

  basic_socketstreambuf()
    : buf(T::eof())
    , sock(){
  }

  ~basic_socketstreambuf(){
  }

  bool connect(const std::string &host, uint16_t port){
    pfi::lang::shared_ptr<stream_socket> sock(new stream_socket());
    this->sock=
      pfi::lang::shared_ptr<socket_holder>
      (new socket_holder_impl<pfi::lang::shared_ptr<stream_socket> >(sock));
    return sock->connect(host, port);
  }

private:
  class socket_holder{
  public:
    virtual ~socket_holder(){}
    virtual stream_socket *get()=0;
  };

  template <class PSOCK>
  class socket_holder_impl : public socket_holder{
  public:
    socket_holder_impl(PSOCK sock): sock(sock) {}
    stream_socket *get(){ return sock.get(); }
  private:
    PSOCK sock;
  };

public:
  template <class PSOCK>
  void setsock(PSOCK sock){
    this->sock=pfi::lang::shared_ptr<socket_holder>(new socket_holder_impl<PSOCK>(sock));
  }

  bool is_connected() const{
    if (!sock->get()) return false;
    return sock->get()->is_connected();
  }

  stream_socket *socket() const{
    return sock->get();
  }

  // virtual protected members

  std::streamsize xsputn(const char_type *s, std::streamsize n){
    if (!sock->get()) return 0;
    int res=sock->get()->write(s, sizeof(char_type)*n);
    if (res<0) return 0;
    return res;
  }

  int overflow(int c){
    if (!sock->get()) return T::eof();
    char d=static_cast<char>(c);
    int res=sock->get()->write(&d, sizeof(char_type));
    if (res<0) return T::eof();
    return res==0?T::eof():0;
  }

  int sync(){
    int res=sock->get()->flush();
    return res<0?0:-1;
  }

  int uflow(){
    int ret=underflow();
    buf=T::eof();
    return ret;
  }

  int underflow(){
    if (buf==T::eof()){
      if (!sock->get()) return T::eof();
      buf=sock->get()->getc();
    }
    return buf;
  }

private:
  int buf;
  pfi::lang::shared_ptr<socket_holder> sock;
};

template <class C, class T = std::char_traits<C> >
class basic_socketstream : public std::basic_iostream<C,T>{
public:
  basic_socketstream()
    : std::basic_iostream<C,T>(){
    this->init(&sockbuf);
  }

  basic_socketstream(const std::string &host, uint16_t port)
    : std::basic_iostream<C,T>(){
    this->init(&sockbuf);
    connect(host, port);
  }

  template <class PSOCK>
  basic_socketstream(PSOCK sock)
    : std::basic_iostream<C,T>(){
    this->init(&sockbuf);
    setsock(sock);
  }

  ~basic_socketstream(){
  }

  void connect(const std::string &host, uint16_t port){
    if (!sockbuf.connect(host, port))
      this->setstate(std::ios_base::failbit);
    else
      this->clear();
  }

  template <class PSOCK>
  void setsock(PSOCK sock){
    sockbuf.setsock(sock);
    this->clear();
  }

  bool is_connected() const{
    return sockbuf.is_connected();
  }

  stream_socket *socket() const{
    return sockbuf.socket();
  }

private:
  basic_socketstreambuf<C,T> sockbuf;
};

typedef basic_socketstreambuf<char> socketstreambuf;
typedef basic_socketstream<char> socketstream;

} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_IOSTREAM_H_
