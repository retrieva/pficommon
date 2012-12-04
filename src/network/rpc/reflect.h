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

#ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_REFLECT_H_
#define INCLUDE_GUARD_PFI_NETWORK_RPC_REFLECT_H_

#include "base.h"
#include "../../data/serialization/reflect.h"

namespace pfi{
namespace network{
namespace rpc{

template <class R>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R()>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  return make_pair(ret, args);
}

template <class R, class A1>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4, class A5>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4,A5)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  args.push_back(pfi::data::serialization::get_type<A5>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4,A5,A6)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  args.push_back(pfi::data::serialization::get_type<A5>());
  args.push_back(pfi::data::serialization::get_type<A6>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  args.push_back(pfi::data::serialization::get_type<A5>());
  args.push_back(pfi::data::serialization::get_type<A6>());
  args.push_back(pfi::data::serialization::get_type<A7>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  args.push_back(pfi::data::serialization::get_type<A5>());
  args.push_back(pfi::data::serialization::get_type<A6>());
  args.push_back(pfi::data::serialization::get_type<A7>());
  args.push_back(pfi::data::serialization::get_type<A8>());
  return make_pair(ret, args);
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
std::pair<pfi::lang::shared_ptr<pfi::data::serialization::type_rep>, std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > >
extract(pfi::lang::function<R(A1,A2,A3,A4,A5,A6,A7,A8,A9)>){
  pfi::lang::shared_ptr<pfi::data::serialization::type_rep> ret=pfi::data::serialization::get_type<R>();
  std::vector<pfi::lang::shared_ptr<pfi::data::serialization::type_rep> > args;
  args.push_back(pfi::data::serialization::get_type<A1>());
  args.push_back(pfi::data::serialization::get_type<A2>());
  args.push_back(pfi::data::serialization::get_type<A3>());
  args.push_back(pfi::data::serialization::get_type<A4>());
  args.push_back(pfi::data::serialization::get_type<A5>());
  args.push_back(pfi::data::serialization::get_type<A6>());
  args.push_back(pfi::data::serialization::get_type<A7>());
  args.push_back(pfi::data::serialization::get_type<A8>());
  args.push_back(pfi::data::serialization::get_type<A9>());
  return make_pair(ret, args);
}

class reflect_base{
public:
  const std::string &get_name() const {
    return name;
  }
  void set_name(const std::string &name){
    this->name=name;
  }

  int get_version() const{
    return version;
  }
  void set_version(int version){
    this->version=version;
  }

  typedef pfi::lang::shared_ptr<pfi::data::serialization::type_rep> type_p;
  typedef std::pair<type_p, std::vector<type_p> > method_type;

  std::map<std::string, method_type> & members(){
    return mem;
  }

protected:
  std::string name;
  int version;
  std::map<std::string, method_type> mem;
};

} // rpc
} // network
} // pfi

#define REFLECT_RPC_PROC(name, ftype) \
  namespace _reflect{ \
  class name : virtual public reflect_base{ \
  public: \
  name(){ \
    mem[#name]=pfi::network::rpc::extract(function<ftype>()); \
  } \
  }; \
  }

#define REFLECT_RPC_GEN(version, base, ...) \
  namespace _reflect{ \
  struct base##_reflect : __VA_ARGS__ { \
  public: \
  base##_reflect(){ \
    set_name(#base); \
    set_version(version); \
  } \
  }; \
  } \
  typedef _reflect::base##_reflect base##_reflect;
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_RPC_REFLECT_H_
