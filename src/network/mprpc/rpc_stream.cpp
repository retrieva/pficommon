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

#include "rpc_stream.h"

namespace pfi {
namespace network {
namespace mprpc {

rpc_stream::rpc_stream(int iofd, double timeout_sec) :
  seqid(0), os(iofd), timeout_sec(timeout_sec) { }

rpc_stream::~rpc_stream() { }


int rpc_stream::try_receive(rpc_message* msg)
{
  msgpack::object obj;
  std::auto_ptr<msgpack::zone> zone;

  int ret = os.read(&obj, &zone, timeout_sec);
  if(ret <= 0) {
    return ret;
  }

  try {
    msg->reset(obj, zone);
  } catch (msgpack::type_error&) {
    return -1;
  }
  return 1;
}

bool rpc_stream::receive(rpc_message* msg)
{
  if(try_receive(msg) <= 0) {
    return false;
  }
  return true;
}


bool rpc_stream::join(uint32_t msgid, rpc_response* result)
{
  while(true) {
    rpc_message msg;
    if(!receive(&msg)) {
      return false;
    }

    if(!msg.is_response()) {
      continue;
    }

    if(msg.msgid() != msgid) {
      continue;
    }

    result->reset(msg);
    return true;
  }
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

