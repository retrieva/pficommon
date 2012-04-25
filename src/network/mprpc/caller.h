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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_CALLER_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_CALLER_H_

#include <sstream>

#include "../../lang/shared_ptr.h"
#include "../../lang/bind.h"
#include "../../lang/function.h"
#include "exception.h"
#include "rpc_stream.h"
#include "argument.h"

namespace pfi {
namespace network {
namespace mprpc {


typedef pfi::lang::function<pfi::lang::shared_ptr<rpc_stream>()> stream_getter;

static void gen_exception(const rpc_response& res)
{
  switch(res.error_code()) {
  case METHOD_NOT_FOUND:
    throw method_not_found(res.result_string());
  case TYPE_MISMATCH:
    throw rpc_type_error("rpc type mismatch");
  default:
    throw rpc_error(res.error_string());
  }
}

#define GET_CONN \
  pfi::lang::shared_ptr<rpc_stream> rs = sg(); \
  if (!rs) throw rpc_io_error("cannot connect: ",errno);

#define DO_RPC(param) \
  rpc_response res; \
  rs->call(name, param, &res); \
  if(res.is_error()) { \
    gen_exception(res); \
  } \
  \
  R ret; \
  if(!res.result_as(&ret)) { \
    throw rpc_type_error("cannot recv rpc result: type error"); \
  } \
  \
  return ret;

template <class R>
class caller0 {
public:
  caller0(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call() {
    GET_CONN;
    argument0 param;
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R>
pfi::lang::function<R()> make_caller(const pfi::lang::function<R()> &, const std::string &name, stream_getter sg)
{
  caller0<R> c(name, sg);
  return pfi::lang::bind(&caller0<R>::call, c);
}

template <class R, class A1>
class caller1 {
public:
  caller1(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1) {
    GET_CONN;
    argument1<A1> param(a1);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1>
pfi::lang::function<R(A1)> make_caller(const pfi::lang::function<R(A1)> &, const std::string &name, stream_getter sg)
{
  caller1<R,A1> c(name, sg);
  return pfi::lang::bind(&caller1<R,A1>::call, c, pfi::lang::_1);
}

template <class R, class A1, class A2>
class caller2 {
public:
  caller2(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2) {
    GET_CONN;
    argument2<A1, A2> param(a1, a2);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2>
pfi::lang::function<R(A1, A2)> make_caller(const pfi::lang::function<R(A1, A2)> &, const std::string &name, stream_getter sg)
{
  caller2<R,A1, A2> c(name, sg);
  return pfi::lang::bind(&caller2<R,A1, A2>::call, c, pfi::lang::_1, pfi::lang::_2);
}

template <class R, class A1, class A2, class A3>
class caller3 {
public:
  caller3(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3) {
    GET_CONN;
    argument3<A1, A2, A3> param(a1, a2, a3);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3>
pfi::lang::function<R(A1, A2, A3)> make_caller(const pfi::lang::function<R(A1, A2, A3)> &, const std::string &name, stream_getter sg)
{
  caller3<R,A1, A2, A3> c(name, sg);
  return pfi::lang::bind(&caller3<R,A1, A2, A3>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3);
}

template <class R, class A1, class A2, class A3, class A4>
class caller4 {
public:
  caller4(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4) {
    GET_CONN;
    argument4<A1, A2, A3, A4> param(a1, a2, a3, a4);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4>
pfi::lang::function<R(A1, A2, A3, A4)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4)> &, const std::string &name, stream_getter sg)
{
  caller4<R,A1, A2, A3, A4> c(name, sg);
  return pfi::lang::bind(&caller4<R,A1, A2, A3, A4>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4);
}

template <class R, class A1, class A2, class A3, class A4, class A5>
class caller5 {
public:
  caller5(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
    GET_CONN;
    argument5<A1, A2, A3, A4, A5> param(a1, a2, a3, a4, a5);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4, class A5>
pfi::lang::function<R(A1, A2, A3, A4, A5)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4, A5)> &, const std::string &name, stream_getter sg)
{
  caller5<R,A1, A2, A3, A4, A5> c(name, sg);
  return pfi::lang::bind(&caller5<R,A1, A2, A3, A4, A5>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, pfi::lang::_5);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
class caller6 {
public:
  caller6(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
    GET_CONN;
    argument6<A1, A2, A3, A4, A5, A6> param(a1, a2, a3, a4, a5, a6);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
pfi::lang::function<R(A1, A2, A3, A4, A5, A6)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6)> &, const std::string &name, stream_getter sg)
{
  caller6<R,A1, A2, A3, A4, A5, A6> c(name, sg);
  return pfi::lang::bind(&caller6<R,A1, A2, A3, A4, A5, A6>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, pfi::lang::_5, pfi::lang::_6);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class caller7 {
public:
  caller7(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
    GET_CONN;
    argument7<A1, A2, A3, A4, A5, A6, A7> param(a1, a2, a3, a4, a5, a6, a7);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7)> &, const std::string &name, stream_getter sg)
{
  caller7<R,A1, A2, A3, A4, A5, A6, A7> c(name, sg);
  return pfi::lang::bind(&caller7<R,A1, A2, A3, A4, A5, A6, A7>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, pfi::lang::_5, pfi::lang::_6, pfi::lang::_7);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class caller8 {
public:
  caller8(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) {
    GET_CONN;
    argument8<A1, A2, A3, A4, A5, A6, A7, A8> param(a1, a2, a3, a4, a5, a6, a7, a8);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8)> &, const std::string &name, stream_getter sg)
{
  caller8<R,A1, A2, A3, A4, A5, A6, A7, A8> c(name, sg);
  return pfi::lang::bind(&caller8<R,A1, A2, A3, A4, A5, A6, A7, A8>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, pfi::lang::_5, pfi::lang::_6, pfi::lang::_7, pfi::lang::_8);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class caller9 {
public:
  caller9(const std::string &name, stream_getter sg) :
    name(name), sg(sg) { }
  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) {
    GET_CONN;
    argument9<A1, A2, A3, A4, A5, A6, A7, A8, A9> param(a1, a2, a3, a4, a5, a6, a7, a8, a9);
    DO_RPC(param);
  }
private:
  std::string name;
  stream_getter sg;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> make_caller(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> &, const std::string &name, stream_getter sg)
{
  caller9<R,A1, A2, A3, A4, A5, A6, A7, A8, A9> c(name, sg);
  return pfi::lang::bind(&caller9<R,A1, A2, A3, A4, A5, A6, A7, A8, A9>::call, c, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, pfi::lang::_5, pfi::lang::_6, pfi::lang::_7, pfi::lang::_8, pfi::lang::_9);
}



}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_CALLER_H_
