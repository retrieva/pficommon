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

#include "base.h"

#include "../../lang/shared_ptr.h"
#include "../../concurrent/thread.h"
#include "../../data/serialization.h"

using namespace pfi::lang;
using namespace pfi::concurrent;
using namespace pfi::data::serialization;

using namespace std;

namespace pfi{
namespace network{
namespace rpc{

static const char *PING_MSG = "<<<PING>>>";
static const char *PONG_MSG = "<<<PONG>>>";

// server

rpc_server::rpc_server(int version)
  :version(version)
{
}

rpc_server::~rpc_server()
{
}

void rpc_server::add(const string &name, const pfi::lang::shared_ptr<invoker_base>& invoker)
{
  funcs[name]=invoker;
}

bool rpc_server::serv(uint16_t port, int nthreads)
{
  pfi::lang::shared_ptr<server_socket> ssock(new server_socket());
  if (!ssock->create(port))
    return false;

  vector<pfi::lang::shared_ptr<thread> > ths(nthreads);
  for (int i=0; i<nthreads; i++){
      ths[i]=pfi::lang::shared_ptr<thread>(new thread(bind(&rpc_server::process, this, ssock)));
    if (!ths[i]->start()) return false;
  }
  for (int i=0; i<nthreads; i++)
    ths[i]->join();
  return true;
}

void rpc_server::process(const pfi::lang::shared_ptr<server_socket>& ssock)
{
  for (;;){
    pfi::lang::shared_ptr<stream_socket> sock(ssock->accept());
    if (!sock) continue;
    sock->set_nodelay(true);

    socketstream ss(sock);
    binary_iarchive ia(ss);
    binary_oarchive oa(ss);
    for (;;){
      string name;
      ia>>name;

      if (!ss)
        break;

      if (name==PING_MSG){
        string t(PONG_MSG);
        oa<<t;
        oa.flush();
        continue;
      }

      if (funcs.count(name)==0){
        string msg(string("NotFound ")+name);
        oa<<msg;
        oa.flush();
        break;
      }

      int req_ver;
      if (ia >> req_ver && req_ver!=version){
        string msg(string("VersionMismatch ")+lexical_cast<string>(version)+" "+lexical_cast<string>(req_ver));
        oa<<msg;
        oa.flush();
        break;
      }

      try{
        funcs[name]->invoke(ia,oa);
      }
      catch(const rpc_error &){
      }
    }
  }
}

rpc_client::rpc_client(const string &host, uint16_t port, int version)
  :host(host), port(port), version(version)
{
}

rpc_client::~rpc_client()
{
}

pfi::lang::shared_ptr<socketstream> rpc_client::get_connection()
{
  for (int i=0;i<2;i++){
    if (!ss || !(*ss)){
      ss=pfi::lang::shared_ptr<socketstream>(new socketstream(host, port));
      if (!(*ss)){
        ss.reset();
        continue;
      }
      if (!ss->socket()->set_nodelay(true)){
        ss.reset();
        continue;
      }
    }

    binary_iarchive ia(*ss);
    binary_oarchive oa(*ss);
    string ping(PING_MSG);
    oa<<ping;
    oa.flush();
    if (!ss){
      ss.reset();
      continue;
    }
    string pong;
    ia>>pong;
    if (!ss){
      ss.reset();
      continue;
    }
    if (pong!=PONG_MSG){
      ss.reset();
      continue;
    }

    break;
  }

  return ss;
}

void rpc_client::return_connection(const pfi::lang::shared_ptr<socketstream>& css)
{
  ss=css;
}

int rpc_client::get_version()
{
  return version;
}

} // rpc
} // network
} // pfi
