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

#ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_BASE_H_
#define INCLUDE_GUARD_PFI_NETWORK_CGI_BASE_H_

#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>

#include "../http.h"

namespace pfi{
namespace network{
namespace cgi{

class clonable{
public:
  virtual ~clonable() {}
  virtual clonable *clone() const {
    throw std::runtime_error("this class cannot clone");
  }
};

template <class T>
class cloner : public virtual clonable{
private:
  clonable *clone() const {
    return new T(dynamic_cast<const T&>(dynamic_cast<const clonable&>(*this)));
  }
};

class cgi : public virtual clonable{
public:
  cgi();
  virtual ~cgi(){}

  virtual void run(std::ostream &out)=0;

  void exec(std::istream &in,
            std::ostream &out,
            std::ostream &err,
            const std::map<std::string, std::string> &cenv);

  void set_max_content_length(size_t length);

protected:
  std::string method;
  std::map<std::string, std::string> env;
  std::map<std::string, std::string> query;
  std::map<std::string, std::vector<std::string> > multi_query;
  std::map<std::string, std::string> file;
  std::map<std::string, pfi::network::http::header> file_info;
  pfi::network::http::header head;

  size_t max_content_length;

private:
  void parse_input(const std::string &os);
  void parse_multipart_input(const std::string &os, const std::string &boundary);
};

} // cgi
} // network
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_BASE_H_
