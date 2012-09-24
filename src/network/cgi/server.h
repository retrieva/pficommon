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

#ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_SERVER_H_
#define INCLUDE_GUARD_PFI_NETWORK_CGI_SERVER_H_

#include "base.h"

#include "../../lang/shared_ptr.h"
#include "../../concurrent/thread.h"

namespace pfi{
namespace network{
namespace cgi{

class run_server{
public:
  typedef pfi::lang::shared_ptr<server_socket> socket_type;

  run_server(const cgi &cc, uint16_t port=8080, int thread_num=1, double time_out=10);
  run_server(const cgi &cc, socket_type ssock, int thread_num=1);
  run_server(const cgi &cc, int argc, char *argv[]);

  ~run_server();

  void run(bool sync=true);
  void join();

private:

  void process(socket_type ssock,
               pfi::lang::shared_ptr<cgi> cc);

  void listen(uint16_t port, double time_out);

  socket_type ssock;
  std::vector<pfi::lang::shared_ptr<pfi::concurrent::thread> > ths;
  int thread_num;
  const cgi &c;
};

class run_server_or_cgi{
public:
  run_server_or_cgi(const cgi &cc, int argc, char *argv[]);
  void run();

private:
  int argc;
  char **argv;
  cgi &c;
};

} // cgi
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_SERVER_H_
