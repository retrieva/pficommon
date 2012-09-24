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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_INVOKER_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_INVOKER_H_

#include "../../lang/shared_ptr.h"
#include "../../lang/function.h"
#include "exception.h"
#include "rpc_stream.h"
#include "message.h"
#include "argument.h"

namespace pfi {
namespace network {
namespace mprpc {


class responder {
public:
  responder(uint32_t msgid, const pfi::lang::shared_ptr<rpc_stream>& rs) :
    msgid(msgid), rs(rs) { }

public:
  template <typename R>
  bool send_result(const R& r)
  {
    return rs->send_response(msgid, r, msgpack::type::nil());
  }

  template <typename E>
  bool send_error(const E& e)
  {
    return rs->send_response(msgid, msgpack::type::nil(), e);
  }

  template <typename E, typename R>
  bool send_error(const E& e, const R& r)
  {
    return rs->send_response(msgid, r, e);
  }

private:
  uint32_t msgid;
  pfi::lang::shared_ptr<rpc_stream> rs;
};


class invoker_base {
public:
  virtual ~invoker_base() {}
  virtual void invoke(rpc_request& req, responder& res) = 0;
};


#define GET_PARAM(req, param) \
  if(!req.param_as(&param)) { \
    res.send_error((unsigned int)TYPE_MISMATCH); \
    throw rpc_type_error("cannot recv argument: type error"); \
  }

#define SEND_RESPONSE(res, param) \
  if(!res.send_result(retval)) { \
    throw rpc_io_error("cannot send return value: ",errno); \
  }

template <class R>
class invoker0 : public invoker_base {
public:
  typedef pfi::lang::function<R()> func_type;
  invoker0(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument0 param;
    GET_PARAM(req, param);
    R retval = f();
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R()> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker0<R>(f));
}

template <class R, class A1>
class invoker1 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1)> func_type;
  invoker1(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument1<A1> param;
    GET_PARAM(req, param);
    R retval = f(param.a1);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker1<R,A1>(f));
}

template <class R, class A1, class A2>
class invoker2 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2)> func_type;
  invoker2(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument2<A1, A2> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker2<R,A1, A2>(f));
}

template <class R, class A1, class A2, class A3>
class invoker3 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3)> func_type;
  invoker3(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument3<A1, A2, A3> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker3<R,A1, A2, A3>(f));
}

template <class R, class A1, class A2, class A3, class A4>
class invoker4 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4)> func_type;
  invoker4(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument4<A1, A2, A3, A4> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker4<R,A1, A2, A3, A4>(f));
}

template <class R, class A1, class A2, class A3, class A4, class A5>
class invoker5 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4, A5)> func_type;
  invoker5(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument5<A1, A2, A3, A4, A5> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4, param.a5);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4, class A5>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4, A5)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker5<R,A1, A2, A3, A4, A5>(f));
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
class invoker6 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4, A5, A6)> func_type;
  invoker6(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument6<A1, A2, A3, A4, A5, A6> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4, param.a5, param.a6);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker6<R,A1, A2, A3, A4, A5, A6>(f));
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class invoker7 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7)> func_type;
  invoker7(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument7<A1, A2, A3, A4, A5, A6, A7> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4, param.a5, param.a6, param.a7);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker7<R,A1, A2, A3, A4, A5, A6, A7>(f));
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class invoker8 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8)> func_type;
  invoker8(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument8<A1, A2, A3, A4, A5, A6, A7, A8> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4, param.a5, param.a6, param.a7, param.a8);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker8<R,A1, A2, A3, A4, A5, A6, A7, A8>(f));
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class invoker9 : public invoker_base {
public:
  typedef pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> func_type;
  invoker9(const func_type &f) : f(f) { }
  virtual void invoke(rpc_request& req, responder& res) {
    argument9<A1, A2, A3, A4, A5, A6, A7, A8, A9> param;
    GET_PARAM(req, param);
    R retval = f(param.a1, param.a2, param.a3, param.a4, param.a5, param.a6, param.a7, param.a8, param.a9);
    SEND_RESPONSE(res, param);
  }
private:
  func_type f;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
pfi::lang::shared_ptr<invoker_base> make_invoker(const pfi::lang::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> &f)
{
  return pfi::lang::shared_ptr<invoker_base>(new invoker9<R,A1, A2, A3, A4, A5, A6, A7, A8, A9>(f));
}



}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_INVOKER_H_
