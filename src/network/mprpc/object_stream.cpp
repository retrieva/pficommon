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

#include "object_stream.h"

#include <memory>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "socket.h"
#include "exception.h"
#include "../../system/time_util.h"

using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace pfi {
namespace network {
namespace mprpc {


object_stream::object_stream(int iofd) :
  iofd(iofd)
{ }

object_stream::object_stream(const std::string& host, uint16_t port) :
  iofd(socket::connect_sock(host, port))
{ }

object_stream::~object_stream()
{
  ::shutdown(iofd, SHUT_RDWR);
  ::close(iofd);
}

int object_stream::read(msgpack::object* obj, std::auto_ptr<msgpack::zone>* zone,
    double timeout_sec)
{
  clock_time start = get_clock_time();
  while(true) {
    if(unpacker.execute()) {
      *obj = unpacker.data();
      zone->reset( unpacker.release_zone() );
      unpacker.reset();
      return 1;
    }

    unpacker.reserve_buffer(1024);

    ssize_t rl;
    while(true) {
      rl = ::read(iofd, unpacker.buffer(), unpacker.buffer_capacity());
      if(rl > 0) break;
      if(rl == 0) { return -1; }
      if(errno == EINTR) { continue; }
      if(timeout_sec < (double)(get_clock_time() - start)){
        throw rpc_timeout_error("timeout");
      }
      if(errno == EAGAIN) { continue; }
      return -1;
    }

    unpacker.buffer_consumed(rl);
  }
}

int object_stream::write(const void* data, size_t size, double timeout_sec)
{
  const char* p = static_cast<const char*>(data);
  const char* const pend = p + size;
  clock_time start = get_clock_time();
  while(p < pend) {
    ssize_t rl = ::write(iofd, p, pend-p);
    if(rl <= 0) {
      if(rl == 0) {
        return -1;
      }
      if(errno == EINTR) { continue; }
      if(timeout_sec < (double)(get_clock_time() - start)){
        throw rpc_timeout_error("timeout");
      }
      if(errno == EAGAIN) { continue; }
      return -1;
    }
    p += rl;
  }
  return size;
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

