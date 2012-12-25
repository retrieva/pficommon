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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_MESSAGE_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_MESSAGE_H_

#include <memory>
#include <sstream>

#include <msgpack.hpp>

namespace pfi {
namespace network {
namespace mprpc {


struct rpc_message {
public:
  rpc_message() { }

  rpc_message(msgpack::object obj, std::auto_ptr<msgpack::zone> z) :
    zone(z)
  {
    obj.convert(&tuple);
  }

  void reset(msgpack::object obj, std::auto_ptr<msgpack::zone> z)
  {
    obj.convert(&tuple);
    zone = z;
  }

  bool is_request()   { return tuple.a0 == 0; }
  bool is_response()  { return tuple.a0 == 1; }

  uint32_t msgid() const { return tuple.a1; }

  msgpack::type::tuple<uint8_t, uint32_t,
    msgpack::object, msgpack::object> tuple;

  std::auto_ptr<msgpack::zone> zone;
};


struct rpc_request {
  rpc_request(rpc_message& msg)
  {
    reset(msg);
  }

  rpc_request()
  { }

  void reset(rpc_message& msg)
  {
    msgid  = msg.tuple.a1;
             msg.tuple.a2.convert(&method);
    param  = msg.tuple.a3;
    zone   = msg.zone;
  }

  template <typename ObjectWritable, typename P>
  static bool write(ObjectWritable& o, uint32_t msgid, const std::string& name, const P& param, double timeout_sec)
  {
    msgpack::type::tuple<uint8_t, uint32_t, const std::string&, const P&>
      req(0, msgid, name, param);
    return o.write(req, timeout_sec) > 0;
  }

  template <typename T>
  bool param_as(T* to)
  try {
    param.convert(to);
    return true;
  } catch (msgpack::type_error&) {
    return false;
  }

  uint32_t msgid;
  std::string method;
  msgpack::object param;
  std::auto_ptr<msgpack::zone> zone;
};


struct rpc_response {
  rpc_response(rpc_message& msg)
  {
    reset(msg);
  }

  rpc_response()
  { }

  void reset(rpc_message& msg)
  {
    msgid  = msg.tuple.a1;
             msg.tuple.a2.convert(&error);
             msg.tuple.a3.convert(&result);
    zone   = msg.zone;
  }

  template <typename ObjectWritable, typename R, typename E>
  static bool write(ObjectWritable& o, uint32_t msgid, const R& retval, const E& error, double timeout_sec)
  {
    msgpack::type::tuple<uint8_t, uint32_t, const E&, const R&>
      res(1, msgid, error, retval);
    return o.write(res, timeout_sec) > 0;
  }

  bool is_error() const
  {
    return !error.is_nil();
  }

  template <typename E>
  bool error_as(E* to) const
  try {
    error.convert(to);
    return true;
  } catch (msgpack::type_error&) {
    return false;
  }

  template <typename R>
  bool result_as(R* to) const
  try {
    result.convert(to);
    return true;
  } catch (msgpack::type_error&) {
    return false;
  }

  unsigned int error_code() const
  {
    if(error.type == msgpack::type::POSITIVE_INTEGER) {
      return error.via.u64;
    }
    return 0;
  }

  std::string error_string() const
  {
    return object_to_string(error);
  }

  std::string result_string() const
  {
    return object_to_string(result);
  }

  uint32_t msgid;
  msgpack::object error;
  msgpack::object result;
  std::auto_ptr<msgpack::zone> zone;

private:
  static std::string object_to_string(msgpack::object o)
  {
    if(o.type == msgpack::type::RAW) {
      return std::string(o.via.raw.ptr, o.via.raw.size);
    }
    std::ostringstream oss;
    oss << o;
    return oss.str();
  }
};


enum error_code {
  METHOD_NOT_FOUND = 1,
  TYPE_MISMATCH    = 2
};


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_MESSAGE_H_
