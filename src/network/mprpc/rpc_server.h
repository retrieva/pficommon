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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_SERVER_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_SERVER_H_

#include <map>
#include <string>

#include "../../lang/shared_ptr.h"
#include "../../lang/function.h"
#include "../../lang/bind.h"
#include "../../concurrent/thread.h"
#include "socket.h"
#include "invoker.h"

namespace pfi {
namespace network {
namespace mprpc {


class basic_server {
public:
  basic_server();
  ~basic_server();

  bool create(uint16_t port, int backlog=4096);
  bool close();

  static bool init_socket(int s);

protected:
  socket sock;
};


class rpc_server : public basic_server {
public:
  rpc_server(double timeout_sec);
  ~rpc_server();

  bool create(uint16_t port, int backlog=4096);
  bool serv(uint16_t port, int nthreads);
  bool run(int nthreads, bool sync = true);
  bool running() const;
  void stop();
  void join();
  void process();


  template <class T>
  void add(const std::string &name, const pfi::lang::function<T> &f);

private:
  double timeout_sec;
  volatile bool serv_running;
  std::vector<pfi::lang::shared_ptr<pfi::concurrent::thread> > serv_threads;

  void add(const std::string &name,
           const pfi::lang::shared_ptr<invoker_base>& invoker);

  void process_request(rpc_request& req, const pfi::lang::shared_ptr<rpc_stream>& rs);

  std::map<std::string, pfi::lang::shared_ptr<invoker_base> > funcs;
};

template <class T>
void rpc_server::add(const std::string &name, const pfi::lang::function<T> &f)
{
  add(name, make_invoker(f));
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_SERVER_H_
