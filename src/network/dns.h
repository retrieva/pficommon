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

#ifndef INCLUDE_GUARD_PFI_NETWORK_DNS_H_
#define INCLUDE_GUARD_PFI_NETWORK_DNS_H_

#include <string>
#include <vector>
#include <stdint.h>

#include "ipv4.h"
#include "../lang/scoped_ptr.h"
#include "../lang/noncopyable.h"

namespace pfi{
namespace network{

class dns_resolver{
public:
  virtual ~dns_resolver(){};

  virtual std::vector<ipv4_address> resolve(const std::string &host, uint16_t port)=0;
};

class normal_dns_resolver : public dns_resolver{
public:
  normal_dns_resolver();
  ~normal_dns_resolver();

  std::vector<ipv4_address> resolve(const std::string &host, uint16_t port);
};

class cached_dns_resolver : public dns_resolver
                          , pfi::lang::noncopyable{
public:
  cached_dns_resolver(int max_size, int expire_second);
  ~cached_dns_resolver();

  void clear_cache();
  void delete_cache(const std::string &host, uint16_t port);

  std::vector<ipv4_address> resolve(const std::string &host, uint16_t port);

private:
  class impl;
  pfi::lang::scoped_ptr<impl> pimpl;
};

} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_DNS_H_
