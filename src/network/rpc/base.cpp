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
static const char *EXIT_MSG = "<<<EXIT>>>";

// TODO: catch exception which std::mutex::mutex() throws
rpc_server::rpc_server(int version, double timeout_sec) : version(version),
                                                          timeout_sec(timeout_sec),
                                                          threads_mutex(),
                                                          num_running_threads(0),
                                                          threads(),
                                                          state_mutex(),
                                                          state(server_state::STOPPED),
                                                          stop_condition() {
  port_num = 0;
}

rpc_server::~rpc_server()
{
  for (size_t i = 0; i < threads.size(); ++i) {
    threads[i]->join();
  }
  if (socket) {
    socket->close();
    socket.reset();
  }
}

void rpc_server::add(const string &name, const pfi::lang::shared_ptr<invoker_base>& invoker)
{
  funcs[name]=invoker;
}

bool rpc_server::serv(uint16_t port, int nthreads)
{
  if (!start(port, nthreads)) {
    return false;
  }
  wait_until_stopped();
  return true;
}

bool rpc_server::start(uint16_t port, int nthreads)
{
  socket = pfi::lang::shared_ptr<server_socket>(new server_socket());
  if (!socket->create(port))
    return false;

  port_num = socket->port();

  // NOTE: state should be set as `running` before starting threads
  //       because each thread runs while the state is `running`.
  set_state(server_state::RUNNING);
  {
    std::lock_guard<std::mutex> lock(threads_mutex);
    threads = vector<pfi::lang::shared_ptr<thread>>(nthreads);
    for (int i=0; i<nthreads; i++){
      threads[i]=pfi::lang::shared_ptr<thread>(new thread(bind(&rpc_server::process, this, socket)));
      if (!threads[i]->start()) return false;
      ++num_running_threads;
    }
  }
  return true;
}

void rpc_server::stop()
{
  if (!is_running()) {
    return;
  }
  set_state(server_state::STOPPING);
  if (socket) {
    socket->close();
    for (size_t i = 0; i < threads.size(); ++i) {
      rpc_client client("localhost", port());
      auto request_termination = client.call<bool()>(EXIT_MSG);
      request_termination();
    }
  }
}

void rpc_server::wait_until_stopped()
{
  if (is_stopped()) {
    return;
  }
  std::unique_lock<std::mutex> lock(state_mutex);
  stop_condition.wait(lock, [this]() { return state == server_state::STOPPED; });
}

bool rpc_server::is_running() const
{
  std::lock_guard<std::mutex> lock(state_mutex);
  return state == server_state::RUNNING;
}

bool rpc_server::is_stopping() const
{
  std::lock_guard<std::mutex> lock(state_mutex);
  return state == server_state::STOPPING;
}

bool rpc_server::is_stopped() const
{
  std::lock_guard<std::mutex> lock(state_mutex);
  return state == server_state::STOPPED;
}

uint16_t rpc_server::port() const
{
  return port_num;
}

void rpc_server::process(const pfi::lang::shared_ptr<server_socket>& ssock)
{
  while (is_running()){
    pfi::lang::shared_ptr<stream_socket> sock(ssock->accept());
    if (!sock) continue;
    sock->set_nodelay(true);
    if (timeout_sec > 0.0) {
      if (!sock->set_timeout(timeout_sec)) {
        continue;
      }
    }

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
      } else if (name==EXIT_MSG) {
        string result_code("OK");
        oa<<result_code;
        oa.flush();
        string ret("1");
        oa<<ret;
        oa.flush();
        break;
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
  {
    std::lock_guard<std::mutex> lock(threads_mutex);
    --num_running_threads;
  }
  if (!exist_running_threads()) {
    set_state(server_state::STOPPED);
    stop_condition.notify_one();
  }
}

bool rpc_server::exist_running_threads() const
{
  std::lock_guard<std::mutex> lock(threads_mutex);
  return num_running_threads > 0;
}

void rpc_server::set_state(const server_state& next_state) {
  std::lock_guard<std::mutex> lock(state_mutex);
  state = next_state;
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
