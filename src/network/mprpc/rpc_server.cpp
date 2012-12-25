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

#include "rpc_server.h"

#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "../../network/socket.h"
#include "../../system/syscall.h"
#include "../../concurrent/thread.h"

namespace pfi {
namespace network {
namespace mprpc {


basic_server::basic_server() { }

basic_server::~basic_server()
{
  close();
}

bool basic_server::close()
{
  return sock.close();
}

bool basic_server::create(uint16_t port, int backlog)
{
  return sock.listen(port, backlog);
}


rpc_server::rpc_server(double timeout_sec) :
  timeout_sec(timeout_sec),
  serv_running(false)
{ }

rpc_server::~rpc_server() { }

bool rpc_server::create(uint16_t port, int backlog)
{
  if (sock.get() >= 0)
    close();

  return basic_server::create(port, backlog);
}

bool rpc_server::serv(uint16_t port, int nthreads)
{
  if (!basic_server::create(port))
    return false;
  return run(nthreads);
}

bool rpc_server::run(int nthreads, bool sync)
{
  using pfi::lang::shared_ptr;
  using pfi::concurrent::thread;

  if (sock.get() < 0 || serv_running)
    return false;

  serv_running = true;
  serv_threads.resize(nthreads);
  for (int i = 0; i < nthreads; i++) {
    serv_threads[i] = shared_ptr<thread>(new thread(
          pfi::lang::bind(&rpc_server::process, this)));
    if (!serv_threads[i]->start()) {
      stop();
      for (int j = 0; j < i; j++) {
        serv_threads[j]->join();
      }
      return false;
    }
  }

  if (sync)
    join();

  return true;
}

bool rpc_server::running() const
{
  return serv_running;
}

void rpc_server::stop()
{
  serv_running = false;
  close();
}

void rpc_server::join()
{
  for (size_t i = 0; i < serv_threads.size(); i++)
    serv_threads[i]->join();
  serv_threads.clear();
}

void rpc_server::process()
{
  while(serv_running) {
    int s;
    NO_INTR(s, ::accept(sock.get(), NULL, NULL));
    if (FAILED(s)) { continue; }
    socket ns(s);

    ns.set_nodelay(true);
    if(timeout_sec > 0) {
      if(!ns.set_timeout(timeout_sec)) {
        continue;
      }
    }

    pfi::lang::shared_ptr<rpc_stream> rs(new rpc_stream(ns.get(), timeout_sec));
    ns.release();

    while(serv_running) {
      rpc_message msg;
      try {
        if(!rs->receive(&msg)) {
          break;
        }
      } catch (rpc_timeout_error&) {
        break;
      }

      if(!msg.is_request()) {
        continue;
      }

      rpc_request req(msg);
      try {
        process_request(req, rs);
      } catch (rpc_error&) {
      }
    }
  }
}

void rpc_server::add(const std::string &name,
                     const pfi::lang::shared_ptr<invoker_base>& invoker)
{
  funcs[name] = invoker;
}

void rpc_server::process_request(rpc_request& req, const pfi::lang::shared_ptr<rpc_stream>& rs)
{
  responder res(req.msgid, rs);

  std::map<std::string, pfi::lang::shared_ptr<invoker_base> >::iterator
    fun = funcs.find(req.method);

  if(fun == funcs.end()) {
    res.send_error((unsigned int)METHOD_NOT_FOUND, req.method);
    return;
  }

  try {
    fun->second->invoke(req, res);
  } catch (rpc_error&) {
  } catch (std::exception& e) {
    res.send_error(std::string(e.what()));
  }
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

