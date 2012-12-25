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

#include "server.h"

#include <vector>

#include <getopt.h>

#include "cgi.h"
#include "util.h"
#include "../http.h"
#include "../../lang/bind.h"
#include "../../pfi-config.h"

extern char **environ;

using namespace std;
using namespace pfi::lang;
using namespace pfi::concurrent;

namespace pfi{
namespace network{
namespace cgi{

run_server::run_server(const cgi &cc, uint16_t port, int thread_num, double time_out)
  : thread_num(thread_num)
  , c(cc)
{
  listen(port, time_out);
}

run_server::run_server(const cgi &cc, int argc, char *argv[])
  : thread_num(1)
  , c(cc)
{
  uint16_t port=8080;
  double time_out=10;

  static option opts[]={
    {"port", required_argument, 0, 'p'},
    {"thread", required_argument, 0, 't'},
    {"timeout", required_argument, 0, 'o'},
    {"help", no_argument, 0, 'h'},
    {"server", no_argument, 0, 's'},
    {0, 0, 0, 0},
  };

  for (;;){
    int option_index=0;
    int c=getopt_long(argc, argv, "p:t:o:hs", &opts[0], &option_index);

    if (c==-1) break;

    switch(c){
    case 'p': {
      int p=atoi(optarg);
      if (p>=1 && p<=0xFFFF)
        port=static_cast<uint16_t>(p);
      else{
        cerr<<"wrong argument: --port="<<optarg<<endl;
        exit(0);
      }
      break;
    }
    case 't': {
      int t=atoi(optarg);
      if (t>=1 && t<=1024)
        thread_num=t;
      else if (t<=0){
        cerr<<"wrong argument: --thread="<<optarg<<endl;
        exit(0);
      }
      else{
        cerr<<"too many threads: --thread="<<optarg<<endl;
        exit(0);
      }
      break;
    }
    case 'o': {
      double t=strtod(optarg, NULL);
      time_out=t;
      break;
    }
    case 'h':{
      cerr<<"usage: "<<argv[0]<<" [-p port] [-t thread-num] [-o timeout] [-h]"<<endl;
      exit(0);
      break;
    }
    case 's':
      break;
      
    }
  }

  listen(port, time_out);
}

run_server::run_server(const cgi &cc, socket_type ssock, int thread_num)
  : ssock(ssock)
  , thread_num(thread_num)
  , c(cc)
{
}

run_server::~run_server()
{
  join();
}

void run_server::listen(uint16_t port, double time_out)
{
  ssock=socket_type(new server_socket());
  if (!ssock->create(port)){
    ostringstream oss;
    oss<<"unable to open port "<<port<<endl;
    ssock.reset();
    throw std::runtime_error(oss.str());
  }
  if (time_out>0 && !ssock->set_timeout(time_out)){
    ostringstream oss;
    oss<<"unable to set timeout"<<port<<endl;
    ssock.reset();
    throw std::runtime_error(oss.str());
  }
}

void run_server::run(bool sync)
{
  if (!ssock) return;

  cout<<"server start: port="<<ssock->port()
      <<", thread-num="<<thread_num
      <<", timeout="<<ssock->timeout()<<endl;

  ths=vector<pfi::lang::shared_ptr<thread> >(thread_num);
  vector<pfi::lang::shared_ptr<cgi> > cgis(thread_num);

  for (int i=0; i<thread_num; i++){
    cgis[i]=pfi::lang::shared_ptr<cgi>(dynamic_cast<cgi*>(c.clone()));
    ths[i]=pfi::lang::shared_ptr<thread>(new thread(bind(&run_server::process, this, ssock, cgis[i])));
    if (!ths[i]->start()){
      ostringstream oss;
      oss<<"unable to start thread"<<endl;
      throw std::runtime_error(oss.str());
    }
  }

  if (sync)
    join();
}

void run_server::join()
{
  for (size_t i=0; i<ths.size(); i++)
    ths[i]->join();
  ths.clear();
}

static pfi::lang::shared_ptr<http::response> gen_resp(stringstream &ss)
{
  http::header head(ss);

  string content_type="text/html; charset=utf-8";
  int code=200;
  string reason="OK";

  if (head.has_key("content-type")){
    content_type=head["content-type"];
    head.erase("content-type");
  }
  if (head.has_key("location")){
    code=302;
    reason="Found";
  }
  if (head.has_key("status")){
    const char *p=head["status"].c_str();
    char *q=NULL;
    code=strtol(p, &q, 10);
    while(*q && isspace(*q)) q++;
    reason=q;
    head.erase("status");
  }
  
  pfi::lang::shared_ptr<http::response> resp(new http::response(code, reason));

  head["Content-Type"]=content_type;

  for (http::header::iterator p=head.begin();
       p!=head.end(); p++)
    resp->head()[p->first]=p->second;

  resp->body()<<ss.rdbuf();

  return resp;
}

string str_to_upper(const string &s)
{
  string ret(s);
  for (size_t i=0; i<s.length(); i++)
    ret[i]=toupper(ret[i]);
  return ret;
}

void run_server::process(socket_type ssock,
                         pfi::lang::shared_ptr<cgi> cc)
{
  for (;;){
    pfi::lang::shared_ptr<stream_socket> sock(ssock->accept());
    if (!sock) continue;

    if (ssock->timeout()>0 && !sock->set_timeout(ssock->timeout()))
      continue;

    try{
      http::request req(sock);
      stringstream sout;

      map<string, string> env;

      for (http::header::iterator p=req.head().begin();
           p!=req.head().end(); p++)
        env["HTTP_"+str_to_upper(p->first)]=p->second;

      env["REQUEST_METHOD"]=req.method();
      env["REQUEST_URI"]=req.path().path();
      if (req.path().query()!="")
        env["REQUEST_URI"]+="?"+req.path().query();
      env["SCRIPT_NAME"]=req.path().path();
      env["QUERY_STRING"]=req.path().query();

      env["REMOTE_ADDR"]=sock->remote_addr().to_string();
      env["REMOTE_PORT"]=lexical_cast<string>(sock->remote_port());

      env["SERVER_PORT"]=lexical_cast<string>(ssock->port());
      env["SERVER_NAME"]="localhost";
      env["SERVER_PROTOCOL"]="HTTP/1.1";
      env["SERVER_SIGNATURE"]="pficommon/" PFICOMMON_VERSION " standalone web server";
      env["SERVER_SOFTWARE"]="pficommon/" PFICOMMON_VERSION;

      env["CONTENT_LENGTH"]=req.head()["Content-Length"];
      env["CONTENT_TYPE"]=req.head()["Content-Type"];

      cc->exec(req.body(), sout, cerr, env);

      pfi::lang::shared_ptr<http::response> resp=gen_resp(sout);
      resp->send(sock);
    }
    catch(exception&){
    }
  }
}

run_server_or_cgi::run_server_or_cgi(const cgi &cc, int argc, char *argv[])
  : argc(argc), argv(argv), c(const_cast<cgi&>(cc))
{
}

void run_server_or_cgi::run()
{
  bool server=false;
  for (int i=1; i<argc; i++)
    if (argv[i]==string("-s")||
        argv[i]==string("--server"))
      server=true;

  if (server){
    run_server(c, argc, argv).run();
  }
  else{
    bool help=false;
    for (int i=1; i<argc; i++)
      if (argv[i]==string("-h")||
          argv[i]==string("--help"))
        help=true;

    if (help){
      cerr<<"usage: "<<argv[0]<<" [-s] [-h] [server-options]"<<endl;
      exit(0);
    }

    run_cgi(c).run();
  }
}

} // cgi
} // network
} // pfi
