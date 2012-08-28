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

#include "fcgi.h"

#include <cstring>

#include "cgi.h"
#include "util.h"

#include "../../pfi-config.h"

#ifdef HAVE_FCGIAPP_H
#include <fcgiapp.h>
#else
#error fcgiapp.h not found
#endif

using namespace std;

namespace pfi{
namespace network{
namespace cgi{

template <class C, class T = std::char_traits<C> >
class basic_fcgxstreambuf : public basic_streambuf<C,T>{
public:
  typedef C char_type;

  basic_fcgxstreambuf(FCGX_Stream *p)
    : p(p)
    , buf(T::eof()){
  }

  std::streamsize xsputn(const char_type *s, std::streamsize n){
    int res=FCGX_PutStr(s, n*sizeof(char_type), p);
    if (res<0) return 0;
    return res;
  }

  int overflow(int c){
    int res=FCGX_PutChar(c, p);
    if (res<0) return T::eof();
    return 0;
  }

  int uflow(){
    int ret=underflow();
    buf=T::eof();
    return ret;
  }

  int underflow(){
    if (buf==T::eof()){
      buf=FCGX_GetChar(p);
      if (buf<0) buf=T::eof();
    }
    return buf;
  }

private:
  FCGX_Stream *p;
  int buf;
};

template <class C, class T = std::char_traits<C> >
class basic_fcgxstream : public basic_iostream<C,T>{
public:
  basic_fcgxstream(FCGX_Stream *p)
    : basic_iostream<C,T>()
    , fcgxbuf(p){
    this->init(&fcgxbuf);
  }

private:
  basic_fcgxstreambuf<C,T> fcgxbuf;
};

typedef basic_fcgxstream<char> fcgxstream;

run_fcgi::run_fcgi(const cgi &cc)
  :c(const_cast<cgi&>(cc)){
}

void run_fcgi::run()
{
  if (FCGX_IsCGI()){
    run_cgi(c).run();
    return;
  }
  
  for (;;){
    FCGX_Stream *in=NULL, *out=NULL, *err=NULL;
    FCGX_ParamArray env=NULL;
    
    int r=FCGX_Accept(&in, &out, &err, &env);
    if (r!=0) break;
    
    fcgxstream ins(in), outs(out), errs(err);
    
    c.exec(ins, outs, errs, get_env(env));

    FCGX_Finish();
  }
}

} // cgi
} // network
} // pfi
