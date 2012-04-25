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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_EXCEPTION_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include "../../system/syscall.h"

namespace pfi {
namespace network {
namespace mprpc {


class rpc_error : public std::exception {
public:
  rpc_error(const std::string &msg): msg(msg) {}
  ~rpc_error() throw() {}
  const char *what() const throw() { return msg.c_str(); }
private:
  std::string msg;
};

class rpc_io_error : public rpc_error {
public:
  rpc_io_error(const std::string& msg, int err)
    : rpc_error(msg+pfi::system::syscall::get_error_msg(err)){}
  ~rpc_io_error() throw() {}
};

class rpc_type_error : public rpc_error {
public:
  rpc_type_error(const std::string& msg)
    : rpc_error(msg){}
  ~rpc_type_error() throw() {}
};

class rpc_timeout_error : public rpc_error {
public:
  rpc_timeout_error(const std::string& msg)
    : rpc_error(msg){}
  ~rpc_timeout_error() throw() {}
};

class method_not_found : public rpc_error {
public:
  method_not_found(const std::string &name)
    : rpc_error(std::string("method \"")+name+"\" is not found"),
      name_(name) {}

  ~method_not_found() throw() {}

  const std::string &name() { return name_; }

private:
  std::string name_;
};


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_EXCEPTION_H_
