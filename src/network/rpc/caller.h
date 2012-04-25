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

#ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_CALLER_H_
#define INCLUDE_GUARD_PFI_NETWORK_RPC_CALLER_H_

#include "../../lang/bind.h"
#include "../../data/serialization.h"

#include "exception.h"

namespace pfi{
namespace network{
namespace rpc{

typedef pfi::lang::function<pfi::lang::shared_ptr<socketstream>()> getter;
typedef pfi::lang::function<void(pfi::lang::shared_ptr<socketstream>)> returner;
typedef pfi::lang::function<int()> versioner;

static void gen_exception(const std::string &str)
{
  std::istringstream iss(str);
  std::string code;
  iss>>code;
  if (code=="NotFound"){
    std::string name;
    iss>>name;
    throw method_not_found(name);
  }
  if (code=="VersionMismatch"){
    int sver, cver;
    iss>>sver>>cver;
    throw version_mismatch(sver, cver);
  }
  throw rpc_error(str);
}

#define GET_CONN \
  pfi::lang::shared_ptr<socketstream> ssp=g(); \
  if (!ssp) throw cannot_get_connection(); \
  socketstream &ss=*ssp; \
  if (!ss) throw cannot_get_connection();

#define DO_RPC(ARG) \
  R ret; \
  int ver=v(); \
  oa << name << ver ARG; \
  if (!ss) throw rpc_error("cannot send arguments"); \
  oa.flush(); \
  std::string result_code; \
  ia >> result_code; \
  if (result_code!="OK") gen_exception(result_code); \
  ia >> ret; \
  if (!ss) throw rpc_error("cannot recv return value");

template <class R>
class caller0{
public:
  caller0(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC();

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1>
class caller1{
public:
  caller1(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2>
class caller2{
public:
  caller2(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3>
class caller3{
public:
  caller3(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4>
class caller4{
public:
  caller4(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4, class A5>
class caller5{
public:
  caller5(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4<<a5);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
class caller6{
public:
  caller6(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4<<a5<<a6);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class caller7{
public:
  caller7(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4<<a5<<a6<<a7);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class caller8{
public:
  caller8(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4<<a5<<a6<<a7<<a8);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class caller9{
public:
  caller9(const std::string &name, getter g, returner r, versioner v)
    :name(name), g(g), r(r), v(v){
  }

  R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9){
    GET_CONN;

    pfi::data::serialization::binary_iarchive ia(ss);
    pfi::data::serialization::binary_oarchive oa(ss);

    DO_RPC(<<a1<<a2<<a3<<a4<<a5<<a6<<a7<<a8<<a9);

    r(ssp);
    return ret;
  }

private:
  std::string name;
  getter g;
  returner r;
  versioner v;
};

#undef GET_CONN
#undef DO_RPC

template <class R>
pfi::lang::function<R()> make_caller
(const pfi::lang::function<R()> &, const std::string &name, getter g, returner r, versioner v){
  caller0<R> c(name,g,r,v);
  return pfi::lang::bind(&caller0<R>::call,c);
}

template <class R, class A1>
pfi::lang::function<R(A1)> make_caller
(const pfi::lang::function<R(A1)> &, const std::string &name, getter g, returner r, versioner v){
  caller1<R,A1> c(name,g,r,v);
  return pfi::lang::bind(&caller1<R,A1>::call,c,pfi::lang::_1);
}

template <class R, class A1, class A2>
pfi::lang::function<R(A1,A2)> make_caller
(const pfi::lang::function<R(A1,A2)> &, const std::string &name, getter g, returner r, versioner v){
  caller2<R,A1,A2> c(name,g,r,v);
  return pfi::lang::bind(&caller2<R,A1,A2>::call,c,pfi::lang::_1,pfi::lang::_2);
}

template <class R, class A1, class A2, class A3>
pfi::lang::function<R(A1,A2,A3)> make_caller
(const pfi::lang::function<R(A1,A2,A3)> &, const std::string &name, getter g, returner r, versioner v){
  caller3<R,A1,A2,A3> c(name,g,r,v);
  return pfi::lang::bind(&caller3<R,A1,A2,A3>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3);
}

template <class R, class A1, class A2, class A3, class A4>
pfi::lang::function<R(A1,A2,A3,A4)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4)> &, const std::string &name, getter g, returner r, versioner v){
  caller4<R,A1,A2,A3,A4> c(name,g,r,v);
  return pfi::lang::bind(&caller4<R,A1,A2,A3,A4>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4);
}

template <class R, class A1, class A2, class A3, class A4, class A5>
pfi::lang::function<R(A1,A2,A3,A4,A5)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4,A5)> &, const std::string &name, getter g, returner r, versioner v){
  caller5<R,A1,A2,A3,A4,A5> c(name,g,r,v);
  return pfi::lang::bind(&caller5<R,A1,A2,A3,A4,A5>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4,pfi::lang::_5);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
pfi::lang::function<R(A1,A2,A3,A4,A5,A6)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4,A5,A6)> &, const std::string &name, getter g, returner r, versioner v){
  caller6<R,A1,A2,A3,A4,A5,A6> c(name,g,r,v);
  return pfi::lang::bind(&caller6<R,A1,A2,A3,A4,A5,A6>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4,pfi::lang::_5,pfi::lang::_6);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7)> &, const std::string &name, getter g, returner r, versioner v){
  caller7<R,A1,A2,A3,A4,A5,A6,A7> c(name,g,r,v);
  return pfi::lang::bind(&caller7<R,A1,A2,A3,A4,A5,A6,A7>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4,pfi::lang::_5,pfi::lang::_6,pfi::lang::_7);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8)> &, const std::string &name, getter g, returner r, versioner v){
  caller8<R,A1,A2,A3,A4,A5,A6,A7,A8> c(name,g,r,v);
  return pfi::lang::bind(&caller8<R,A1,A2,A3,A4,A5,A6,A7,A8>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4,pfi::lang::_5,pfi::lang::_6,pfi::lang::_7,pfi::lang::_8);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8,A9)> make_caller
(const pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8,A9)> &, const std::string &name, getter g, returner r, versioner v){
  caller9<R,A1,A2,A3,A4,A5,A6,A7,A8,A9> c(name,g,r,v);
  return pfi::lang::bind(&caller9<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>::call,c,pfi::lang::_1,pfi::lang::_2,pfi::lang::_3,pfi::lang::_4,pfi::lang::_5,pfi::lang::_6,pfi::lang::_7,pfi::lang::_8,pfi::lang::_9);
}

} // rpc
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_CALLER_H_
