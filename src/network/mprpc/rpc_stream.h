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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_STREAM_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_STREAM_H_

#include "object_stream.h"
#include "message.h"
#include "exception.h"

namespace pfi {
namespace network {
namespace mprpc {


class rpc_stream {
public:
  rpc_stream(int iofd, double timeout_sec);
  rpc_stream(const std::string& host, uint16_t port, double timeout_sec);
  ~rpc_stream();

public:
  template <typename P>
  void call(const std::string& name, const P& param, rpc_response* result);

  template <typename P>
  bool send(const std::string& name, const P& param, uint32_t* msgid);

  bool join(uint32_t msgid, rpc_response* result);

  int try_receive(rpc_message* msg);
  bool receive(rpc_message* msg);

  template <typename R, typename E>
  bool send_response(uint32_t msgid, const R& retval, const E& error);

private:
  uint32_t seqid;
  object_stream os;
  double timeout_sec;
};


template <typename P>
bool rpc_stream::send(const std::string& name, const P& param, uint32_t* msgid)
{
  *msgid = seqid++;

  if(!rpc_request::write(os, *msgid, name, param, timeout_sec)) {
    return false;
  }

  return true;
}


template <typename P>
void rpc_stream::call(const std::string& name, const P& param, rpc_response* result)
{
  uint32_t msgid;
  if(!send(name, param, &msgid)) {
    throw rpc_io_error("cannot send rpc request: ",errno);
  }

  if(!join(msgid, result)) {
    throw rpc_error("cannot receive rpc result");
  }
}


template <typename R, typename E>
bool rpc_stream::send_response(uint32_t msgid, const R& retval, const E& error)
{
  return rpc_response::write(os, msgid, retval, error, timeout_sec);
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_RPC_STREAM_H_
