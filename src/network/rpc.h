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

#ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_H_
#define INCLUDE_GUARD_PFI_NETWORK_RPC_H_

#include "rpc/base.h"

#define RPC_PROC(name, ...)					\
  namespace _server_impl{					\
  class name : public virtual pfi::network::rpc::rpc_server{	\
  public:							\
  name():rpc_server(){}						\
  void set_##name(const pfi::lang::function<__VA_ARGS__> &f){	\
    rpc_server::add<__VA_ARGS__>(#name, f);			\
  }								\
  };								\
  }								\
                                                                \
  namespace _client_impl{					\
  class name : public virtual pfi::network::rpc::rpc_client{	\
  public:							\
  name()							\
    :rpc_client("",0){						\
    call_##name=call<__VA_ARGS__>(#name);			\
  }								\
  pfi::lang::function<__VA_ARGS__> call_##name;			\
  };								\
  }
  
#define RPC_GEN(ver, base, ...)						\
  namespace _server_impl{						\
  struct base##_server : __VA_ARGS__ {					\
  public:								\
  base##_server()							\
  :rpc_server(ver){							\
  }									\
  };									\
  }									\
  typedef _server_impl::base##_server base##_server;			\
  									\
  namespace _client_impl{						\
  struct base##_client : __VA_ARGS__ {					\
  public:								\
  base##_client(const std::string &host, uint16_t port)			\
  :rpc_client(host, port, ver){						\
  }									\
  };									\
  }									\
  typedef _client_impl::base##_client base##_client;

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_H_
