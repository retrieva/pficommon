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
	timeout_sec(timeout_sec) { }

rpc_server::~rpc_server() { }


bool rpc_server::serv(uint16_t port, int nthreads)
{
	using pfi::lang::shared_ptr;
	using pfi::concurrent::thread;

	if (!basic_server::create(port))
		return false;

	std::vector<shared_ptr<thread> > ths(nthreads);
	for (int i=0; i<nthreads; i++) {
		ths[i] = shared_ptr<thread>(new thread(
					pfi::lang::bind(&rpc_server::process, this)));
		if (!ths[i]->start()) return false;
	}

	for (int i=0; i<nthreads; i++) {
		ths[i]->join();
	}

	return true;
}

void rpc_server::process()
{
	while(true) {
		int s;
		NO_INTR(s, ::accept(sock.get(), NULL, NULL));
		if (FAILED(s)) { continue; }
		socket ns(s);

		ns.set_nodelay(true);
		if(timeout_sec > 0) {
			if(!sock.set_timeout(timeout_sec)) {
				continue;
			}
		}

		pfi::lang::shared_ptr<rpc_stream> rs(new rpc_stream(ns.get(), timeout_sec));
		ns.release();

		while(true) {
			rpc_message msg;
			if(!rs->receive(&msg)) {
				break;
			}

			if(!msg.is_request()) {
				continue;
			}

			rpc_request req(msg);
			try {
				process_request(req, rs);
			} catch (const rpc_error&e) {
			}
		}
	}
}

void rpc_server::add(const std::string &name,
		pfi::lang::shared_ptr<invoker_base> invoker)
{
	funcs[name] = invoker;
}

void rpc_server::process_request(rpc_request& req, pfi::lang::shared_ptr<rpc_stream> rs)
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
        } catch (const rpc_error &e) {
        } catch (const std::exception &e) {
                res.send_error(std::string(e.what()));
        }
}


}  // namespace mprpc
}  // namespace network
}  // namespace pfi

