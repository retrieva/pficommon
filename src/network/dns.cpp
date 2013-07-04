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

#include "dns.h"

#include <set>
#include <map>
#include <cassert>

#include <netdb.h>

#include "../lang/cast.h"
#include "../concurrent/mutex.h"

using namespace std;

using namespace pfi::lang;
using namespace pfi::concurrent;

namespace pfi{
namespace network{

normal_dns_resolver::normal_dns_resolver()
{
}

normal_dns_resolver::~normal_dns_resolver()
{
}

vector<ipv4_address> normal_dns_resolver::resolve(const string &host, uint16_t port)
{
  try{
    vector<ipv4_address> ret;

    addrinfo hints={};
    hints.ai_family=PF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_protocol=0;

    addrinfo *res=NULL;
    if (getaddrinfo(host.c_str(),
          lexical_cast<string>(port).c_str(),
          &hints,&res)!=0)
      return vector<ipv4_address>();

    for (addrinfo *p=res;p!=NULL;p=p->ai_next){
      char ips[32]; // xxx.xxx.xxx.xxx
                    // its length is less than 15+1 < 32
      if (getnameinfo(p->ai_addr,p->ai_addrlen,
            ips,32,
            NULL,0,
            NI_NUMERICHOST)!=0)
        continue;

      ipv4_address ip(ips);
      if (ip!=ipv4_address::none)
        ret.push_back(ip);
    }

    freeaddrinfo(res);

    return ret;
  }
  catch(std::bad_cast&){
    // never come
    return vector<ipv4_address>();
  }
}

class cached_dns_resolver::impl{
public:
  impl(int max_size, int expire_second);
  ~impl();

  void clear_cache();
  void delete_cache(const std::string &host, uint16_t port);

  vector<ipv4_address> resolve(const string &host, uint16_t port);

private:
  int max_size;
  int expire_second;

  std::map<std::pair<std::string, uint16_t>,
    std::pair<std::vector<ipv4_address>, time_t> > cache;

  std::multiset<std::pair<time_t, std::pair<std::string, uint16_t> > > access_time;

  normal_dns_resolver norm;
  r_mutex m;
};

cached_dns_resolver::cached_dns_resolver(int max_size, int expire_second)
  :pimpl(new impl(max_size,expire_second))
{
}

cached_dns_resolver::~cached_dns_resolver()
{
}

void cached_dns_resolver::clear_cache()
{
  pimpl->clear_cache();
}

void cached_dns_resolver::delete_cache(const string &host, uint16_t port)
{
  pimpl->delete_cache(host, port);
}

vector<ipv4_address> cached_dns_resolver::resolve(const string &host, uint16_t port)
{
  return pimpl->resolve(host, port);
}

cached_dns_resolver::impl::impl(int max_size, int expire_second)
  :max_size(max_size), expire_second(expire_second)
{
}

cached_dns_resolver::impl::~impl()
{
}

void cached_dns_resolver::impl::clear_cache()
{
  pfi::concurrent::scoped_lock lock(m);
  if (lock) {
    cache.clear();
    access_time.clear();
  }
}

void cached_dns_resolver::impl::delete_cache(const string &host, uint16_t port)
{
  pfi::concurrent::scoped_lock lock(m);
  if (lock) {
    pair<string,uint16_t> key(host,port);
    time_t del_time=cache[key].second;
    access_time.erase(make_pair(del_time,key));
    cache.erase(key);
  }
}

vector<ipv4_address> cached_dns_resolver::impl::resolve(const string &host, uint16_t port)
{
  // DNS resolver using LRU
  // O(log max_size)

  time_t cur=time(NULL);

  pair<string,uint16_t> key(host,port);

  {
    pfi::concurrent::scoped_lock lock(m);
    if (lock) {
      // hit cache
      if (cache.count(key) != 0) {
        // last access time
        access_time.erase(make_pair(cache[key].second, key));
        access_time.insert(make_pair(cur, key));
        cache[key].second = cur;

        // is not expire
        if (cur-cache[key].second < expire_second) {
          return cache[key].first;
        }

        // expire, so delete cache
        access_time.erase(make_pair(cur, key));
        cache.erase(key);
      }
    }
  }

  // normal DNS resolving
  // it might be do concurrent
  vector<ipv4_address> ret=norm.resolve(host,port);

  {
    pfi::concurrent::scoped_lock lock(m);
    if (lock) {
      // is cache full?
      while ((int)cache.size() >= max_size) {
        assert(access_time.size() == cache.size());
        // oldest entry must be head, so delete it
        pair<string,uint16_t> del_key = access_time.begin()->second;
        cache.erase(del_key);
        access_time.erase(access_time.begin());
      }

      // register to cache
      access_time.insert(make_pair(cur, key));
      cache[key] = make_pair(ret, cur);
    }
  }

  return ret;
}

} // network
} // pfi
