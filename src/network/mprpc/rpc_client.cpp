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

#include "rpc_client.h"

#include "socket.h"

namespace pfi {
namespace network {
namespace mprpc {


rpc_client::rpc_client(const std::string& host, uint16_t port, double timeout_sec) :
  host(host), port(port), timeout_sec(timeout_sec)
{ }

rpc_client::~rpc_client() { }


pfi::lang::shared_ptr<rpc_stream> rpc_client::get_connection()
{
  if(ss) { return ss; }

  for (int i=0; i < 2; i++){
    socket sock;
    if(!sock.connect(host, port)) {
      continue;
    }

    if(timeout_sec > 0) {
      if(!sock.set_timeout(timeout_sec)) {
        continue;
      }
    }

    if(!sock.set_nodelay(true)) {
      continue;
    }

    ss.reset( new rpc_stream(sock.get(), timeout_sec) );
    sock.release();

    break;
  }

  return ss;
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

