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

#ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_BASE_H_
#define INCLUDE_GUARD_PFI_NETWORK_RPC_BASE_H_

#include <map>
#include <string>
#include <stdexcept>

#include "../../lang/bind.h"
#include "../../lang/function.h"
#include "../../lang/shared_ptr.h"
#include "../../concurrent/thread.h"
#include "../../data/serialization.h"
#include "../../network/socket.h"
#include "../../network/iostream.h"
#include "invoker.h"
#include "caller.h"

#include <map>
#include <string>
#include <stdexcept>

namespace pfi{
namespace network{
namespace rpc{

class rpc_server{
public:
  rpc_server(int version=0);
  ~rpc_server();

  template <class T>
  void add(const std::string &name, const pfi::lang::function<T> &f){
    add(name, make_invoker(f));
  }

  bool serv(uint16_t port, int nthreads);

private:
  void add(const std::string &name,
           const pfi::lang::shared_ptr<invoker_base>& invoker);

  void process(const pfi::lang::shared_ptr<server_socket>& sock);

  std::map<std::string, pfi::lang::shared_ptr<invoker_base> > funcs;

  const int version;
};

class rpc_client{
public:
  rpc_client(const std::string &host, uint16_t port, int version=0);
  ~rpc_client();

  template <class T>
  pfi::lang::function<T> call(const std::string &name){
    return make_caller(pfi::lang::function<T>(), name,
        pfi::lang::bind(&rpc_client::get_connection, this),
        pfi::lang::bind(&rpc_client::return_connection, this, pfi::lang::_1),
        pfi::lang::bind(&rpc_client::get_version, this));
  }

private:
  pfi::lang::shared_ptr<socketstream> get_connection();
  void return_connection(const pfi::lang::shared_ptr<socketstream>& css);

  int get_version();

  std::string host;
  uint16_t port;

  pfi::lang::shared_ptr<socketstream> ss;
  const int version;
};

} // rpc
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_BASE_H_
