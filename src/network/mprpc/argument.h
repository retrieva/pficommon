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

#ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_ARGUMENT_H_
#define INCLUDE_GUARD_PFI_NETWORK_MPRPC_ARGUMENT_H_

#include <msgpack.hpp>
#include <algorithm>

namespace pfi {
namespace network {
namespace mprpc {


struct argument0 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(0);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
  }
};

template <typename A1>
struct argument1 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(1);
    o.pack(a1);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
  }

  argument1() { }

  argument1(A1 _a1) : a1(_a1) { }

  A1 a1;
};

template <typename A1, typename A2>
struct argument2 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(2);
    o.pack(a1);
    o.pack(a2);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
  }

  argument2() { }

  argument2(A1 _a1, A2 _a2) : a1(_a1), a2(_a2) { }

  A1 a1;
  A2 a2;
};

template <typename A1, typename A2, typename A3>
struct argument3 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(3);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
  }

  argument3() { }

  argument3(A1 _a1, A2 _a2, A3 _a3) : a1(_a1), a2(_a2), a3(_a3) { }

  A1 a1;
  A2 a2;
  A3 a3;
};

template <typename A1, typename A2, typename A3, typename A4>
struct argument4 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(4);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
  }

  argument4() { }

  argument4(A1 _a1, A2 _a2, A3 _a3, A4 _a4) : a1(_a1), a2(_a2), a3(_a3), a4(_a4) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5>
struct argument5 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(5);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
    o.pack(a5);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
    if(sz <= 4) { return; } o.via.array.ptr[4].convert(&a5);
  }

  argument5() { }

  argument5(A1 _a1, A2 _a2, A3 _a3, A4 _a4, A5 _a5) : a1(_a1), a2(_a2), a3(_a3), a4(_a4), a5(_a5) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct argument6 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(6);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
    o.pack(a5);
    o.pack(a6);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
    if(sz <= 4) { return; } o.via.array.ptr[4].convert(&a5);
    if(sz <= 5) { return; } o.via.array.ptr[5].convert(&a6);
  }

  argument6() { }

  argument6(A1 _a1, A2 _a2, A3 _a3, A4 _a4, A5 _a5, A6 _a6) : a1(_a1), a2(_a2), a3(_a3), a4(_a4), a5(_a5), a6(_a6) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
  A6 a6;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct argument7 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(7);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
    o.pack(a5);
    o.pack(a6);
    o.pack(a7);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
    if(sz <= 4) { return; } o.via.array.ptr[4].convert(&a5);
    if(sz <= 5) { return; } o.via.array.ptr[5].convert(&a6);
    if(sz <= 6) { return; } o.via.array.ptr[6].convert(&a7);
  }

  argument7() { }

  argument7(A1 _a1, A2 _a2, A3 _a3, A4 _a4, A5 _a5, A6 _a6, A7 _a7) : a1(_a1), a2(_a2), a3(_a3), a4(_a4), a5(_a5), a6(_a6), a7(_a7) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
  A6 a6;
  A7 a7;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct argument8 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(8);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
    o.pack(a5);
    o.pack(a6);
    o.pack(a7);
    o.pack(a8);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
    if(sz <= 4) { return; } o.via.array.ptr[4].convert(&a5);
    if(sz <= 5) { return; } o.via.array.ptr[5].convert(&a6);
    if(sz <= 6) { return; } o.via.array.ptr[6].convert(&a7);
    if(sz <= 7) { return; } o.via.array.ptr[7].convert(&a8);
  }

  argument8() { }

  argument8(A1 _a1, A2 _a2, A3 _a3, A4 _a4, A5 _a5, A6 _a6, A7 _a7, A8 _a8) : a1(_a1), a2(_a2), a3(_a3), a4(_a4), a5(_a5), a6(_a6), a7(_a7), a8(_a8) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
  A6 a6;
  A7 a7;
  A8 a8;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct argument9 {
  template <typename Packer>
  void msgpack_pack(Packer& o) const
  {
    o.pack_array(9);
    o.pack(a1);
    o.pack(a2);
    o.pack(a3);
    o.pack(a4);
    o.pack(a5);
    o.pack(a6);
    o.pack(a7);
    o.pack(a8);
    o.pack(a9);
  }

  void msgpack_unpack(msgpack::object o)
  {
    if(o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
    const size_t sz = o.via.array.size;
    if(sz <= 0) { return; } o.via.array.ptr[0].convert(&a1);
    if(sz <= 1) { return; } o.via.array.ptr[1].convert(&a2);
    if(sz <= 2) { return; } o.via.array.ptr[2].convert(&a3);
    if(sz <= 3) { return; } o.via.array.ptr[3].convert(&a4);
    if(sz <= 4) { return; } o.via.array.ptr[4].convert(&a5);
    if(sz <= 5) { return; } o.via.array.ptr[5].convert(&a6);
    if(sz <= 6) { return; } o.via.array.ptr[6].convert(&a7);
    if(sz <= 7) { return; } o.via.array.ptr[7].convert(&a8);
    if(sz <= 8) { return; } o.via.array.ptr[8].convert(&a9);
  }

  argument9() { }

  argument9(A1 _a1, A2 _a2, A3 _a3, A4 _a4, A5 _a5, A6 _a6, A7 _a7, A8 _a8, A9 _a9) : a1(_a1), a2(_a2), a3(_a3), a4(_a4), a5(_a5), a6(_a6), a7(_a7), a8(_a8), a9(_a9) { }

  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
  A6 a6;
  A7 a7;
  A8 a8;
  A9 a9;
};



}  // namespace mprpc
}  // namespace network
}  // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_MPRPC_ARGUMENT_H_
