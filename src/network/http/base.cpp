// Copyright (c)2008-2013, Preferred Infrastructure Inc.
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

#include <cstdio>
#include <algorithm>
#include <stdint.h>

#include "../iostream.h"
#include "../../lang/bind.h"

using namespace std;
using namespace pfi::lang;

const static int buf_size = 4096;
const static int line_limit = 4096;

namespace pfi {
namespace network {
namespace http {

static bool cicmp(const string& s, const string& t)
{
  if (s.length() != t.length()) return false;

  for (size_t i = 0; i < s.length(); i++)
    if (tolower(s[i]) != tolower(t[i]))
      return false;
  return true;
}

header::header()
{
}

void header::read_header(pfi::lang::function<bool (string*)> f)
{
  string line, key, val;
  if (!f(&line))
    throw http_exception("invalid header");

  if (line.length() == 0) return;

  bool cont = true;
  while (cont) {
    // field-name ":" [field-value]
    for (size_t i = 0; i < line.length(); i++) {
      if (line[i] != ':') {
        key += line[i];
      } else {
        int ed = key.length();
        while (ed > 0 && isspace(key[ed-1]))
          ed--;
        key = key.substr(0,ed);

        val = line.substr(i+1);
        break;
      }
    }

    // LWS
    for (;;) {
      if (!f(&line)) {
        cont = false;
        break;
      }
      if (line.length() == 0 || line == "\r") {
        cont = false;
        break;
      }
      if (isspace(line[0])) {
        // LWS
        for (size_t i = 0; i < line.length(); i++) {
          if (val.length() > 0 &&
              isspace(val[val.length()-1]) &&
              isspace(line[i]));
          else
            val += line[i];
        }
      }
      else
        break;
    }

    size_t st = 0;
    while (st < val.length() && isspace(val[st])) st++;

    set(key, val.substr(st));
    key = "";
    val = "";
  }
}

static bool socket_getline(stream_socket* sock, string* str, int line_limit)
{
  return sock->getline(*str, line_limit);
}

header::header(const pfi::lang::shared_ptr<stream_socket>& sock)
{
  read_header(bind(&socket_getline, sock.get(), _1, line_limit));
}

static bool istream_getline(istream* is, string* str)
{
  return getline(*is, *str);
}

header::header(istream& is)
{
  read_header(bind(&istream_getline, &is, _1));
}

header::~header()
{
}

bool header::has_key(const string& key) const
{
  for (size_t i = 0; i < dat.size(); i++)
    if (cicmp(dat[i].first, key))
      return true;
  return false;
}

const string& header::get(const string& key)
{
  return (*this)[key];
}

void header::set(const string& key, const string& val)
{
  (*this)[key]=val;
}

void header::erase(const string& key)
{
  for (iterator it = dat.begin(); it != dat.end(); ++it) {
    if (cicmp(it->first,key)) {
      dat.erase(it);
      return;
    }
  }
}

std::string& header::operator[](const string& key)
{
  for (size_t i = 0; i < dat.size(); i++)
    if (cicmp(dat[i].first, key))
      return dat[i].second;

  dat.push_back(make_pair(key,""));
  return dat.back().second;
}

const std::string& header::operator[](const string& key) const
{
  for (size_t i = 0; i < dat.size(); i++)
    if (cicmp(dat[i].first,key))
      return dat[i].second;

  static const string null_str;
  return null_str;
}

header::iterator header::begin()
{
  return dat.begin();
}

header::const_iterator header::begin() const
{
  return dat.begin();
}

header::iterator header::end()
{
  return dat.end();
}

header::const_iterator header::end() const
{
  return dat.end();
}

void header::send(const pfi::lang::shared_ptr<stream_socket>& sock)
{
  for (size_t i = 0; i < dat.size(); i++) {
    string line = dat[i].first + ": " + dat[i].second + "\r\n";
    if (!sock->puts(line))
      throw http_exception("cannot send header");
  }
  sock->puts("\r\n");
}

// only support under C=char.

template <class C, class T = char_traits<C> >
class basic_httpbody_chunked_streambuf : public basic_streambuf<C, T> {
public:
  typedef C char_type;

  explicit basic_httpbody_chunked_streambuf(const pfi::lang::shared_ptr<stream_socket>& sock)
    : sock(sock),
      chunk_rest(0),
      buf(buf_size),
      buf_pos(0),
      buf_end(0)
  {}

  int uflow() {
    int ret = underflow();
    inc();
    return ret;
  }

  int underflow() {
    if (empty())
      fill();
    if (empty())
      return T::eof();
    return buf[buf_pos];
  }

private:
  void shutdown() {
    sock.reset();
    chunk_rest = 0;
    buf_pos = 0;
    buf_end = 0;
  }

  bool empty() const {
    return buf_pos >= buf_end;
  }

  void inc() {
    if (!empty()) buf_pos++;
  }

  void fill() {
    if (!sock) return;

    if (chunk_rest == 0)
      read_chunk_size();

    if (chunk_rest == 0) { // last chunk
      // skip trailer
      if (sock) {
        for (string line;
             sock->getline(line,line_limit) && line.length()>0;);
      }
      shutdown();
      return;
    }

    int cur = min(chunk_rest, buf_size);
    int rbyte = sock->read(&buf[0], cur);

    if (rbyte != cur) {
      shutdown();
      return;
    }

    buf_pos = 0;
    buf_end = cur;
    chunk_rest -= cur;

    if (chunk_rest == 0) {
      char c = sock->getc();
      if (c == '\r'){
        c = sock->getc();
        if (c != '\n')
          throw http_exception("invalid chunk footer");
      }
      else if (c != '\n')
        throw http_exception("invalid chunk footer");
    }
  }

  void read_chunk_size() {
    if (!sock) return;

    string line;
    if (!sock->getline(line, line_limit)) {
      shutdown();
      return;
    }

    // line = chunk-size [chunk-extension] CRLF

    istringstream iss(line);
    iss >> hex >> chunk_rest;
  }

  pfi::lang::shared_ptr<stream_socket> sock;

  int chunk_rest;

  vector<C> buf;
  int buf_pos, buf_end;
};

template <class C, class T = char_traits<C> >
class basic_httpbody_streambuf : public basic_streambuf<C, T> {
public:
  typedef C char_type;

  basic_httpbody_streambuf(const pfi::lang::shared_ptr<stream_socket>& sock, int64_t length)
    : sock(sock),
      rest(length),
      buf(T::eof())
  {}

  int uflow() {
    int ret = underflow();
    buf = T::eof();
    return ret;
  }

  int underflow() {
    if (buf == T::eof()) {
      if (!sock)
        return T::eof();
      buf = sock->getc();
      if (--rest == 0)
        sock.reset();
    }
    return buf;
  }

private:
  pfi::lang::shared_ptr<stream_socket> sock;

  int64_t rest;
  int buf;
};

template <class C, class T = char_traits<C> >
class basic_httpbody_chunked_stream : public basic_iostream<C, T> {
public:
  explicit basic_httpbody_chunked_stream(const pfi::lang::shared_ptr<stream_socket>& sock)
    : basic_iostream<C,T>(), buf(sock)
  {
    this->init(&buf);
  }

private:
  basic_httpbody_chunked_streambuf<C, T> buf;
};

template <class C, class T = char_traits<C> >
class basic_httpbody_stream : public basic_iostream<C, T> {
public:
  basic_httpbody_stream(const pfi::lang::shared_ptr<stream_socket>& sock, int64_t len)
    : basic_iostream<C,T>(),
      buf(sock, len)
  {
    this->init(&buf);
  }
private:
  basic_httpbody_streambuf<C, T> buf;
};

const string method::get     = "GET";
const string method::post    = "POST";
const string method::head    = "HEAD";
const string method::put     = "PUT";
const string method::del     = "DELETE";
const string method::trace   = "TRACE";
const string method::connect = "CONNECT";
const string method::options = "OPTIONS";

request::request(const string& method, const uri& u, int major_ver, int minor_ver)
  : method_(method),
    uri_(u),
    version_(major_ver,minor_ver),
    stream(new stringstream())
{}

request::request(const pfi::lang::shared_ptr<stream_socket>& sock)
  : method_(""),
    uri_("/"),
    version_(1,1),
    stream()
{
  // Request-Line
  {
    string head;
    sock->getline(head, line_limit);
  
    istringstream iss(head);
    string uri_s;
    string ver;
    iss >> method_ >> uri_s >> ver;
    uri_ = pfi::network::uri(uri_s);

    for (size_t i = 0; i < ver.length(); i++)
      ver[i] = toupper(ver[i]);

    int ma, mi;
    if (sscanf(ver.c_str(), "HTTP/%d.%d", &ma, &mi) != 2)
      throw http_exception("invalid request-line");

    version_ = make_pair(ma,mi);
  }

  // header
  header_ = header(sock);

  // body
  if (cicmp(header_["Transfer-Encoding"], "chunked"))
    stream = pfi::lang::shared_ptr<iostream>(new basic_httpbody_chunked_stream<char>(sock));
  else if (header_["Content-Length"] != "")
    stream = pfi::lang::shared_ptr<iostream>(new basic_httpbody_stream<char>(sock, lexical_cast<int64_t>(header_["Content-Length"])));
  else
    stream = pfi::lang::shared_ptr<iostream>(new socketstream(sock));
}

request::~request() {}

const string& request::method() const
{
  return method_;
}

const uri& request::path() const
{
  return uri_;
}

const pair<int,int>& request::version() const
{
  return version_;
}

header& request::head()
{
  return header_;
}

iostream& request::body()
{
  return *stream;
}

void request::send(const pfi::lang::shared_ptr<stream_socket>& sock)
{
  stringstream* ss = dynamic_cast<stringstream*>(stream.get());
  if (!ss)
    throw http_exception("body is not stringstream");
  string dat = ss->str();

  string req_line =
    method_ + " " +
    lexical_cast<string>(uri_) + " " +
    "HTTP/" + lexical_cast<string>(version_.first) + "." + lexical_cast<string>(version_.second) + "\r\n";

  if (!sock->puts(req_line))
    throw http_exception("cannot send reqest-line");

  if (!header_.has_key("Content-Length") && dat.length() > 0)
    header_["Content-Length"] = lexical_cast<string>(dat.length());
  header_.send(sock);

  if (dat.length() > 0) {
    if (!sock->puts(dat))
      throw http_exception("cannot send body");
  }
  if (sock->flush() >= 0)
    throw http_exception("flush failed");
  if (dat.length() == 0)
    stream = pfi::lang::shared_ptr<iostream>(new socketstream(sock));
}

response::response()
  : version_(1, 1),
    code_(0),
    reason_(""),
    stream(new stringstream())
{}

response::response(int code, const string &reason, int major_ver, int minor_ver)
  : version_(major_ver, minor_ver),
    code_(code),
    reason_(reason),
    stream(new stringstream())
{}

response::response(const pfi::lang::shared_ptr<stream_socket>& sock)
{
  // status-line
  {
    string line;
    if (!sock->getline(line, line_limit))
      throw http_exception("cannot read status-line: " + line);
    
    istringstream iss(line);
    string ver;
    iss >> ver >> ws >> code_ >> ws;

    getline(iss, reason_);

    for (size_t i = 0; i < ver.length(); i++)
      ver[i] = toupper(ver[i]);

    int ma, mi;
    if (sscanf(ver.c_str(), "HTTP/%d.%d", &ma, &mi) != 2)
      throw http_exception("invalid status-line");
    version_ = make_pair(ma, mi);
  }

  // header
  header_ = header(sock);

  // body
  if (cicmp(header_["Transfer-Encoding"], "chunked"))
    stream = pfi::lang::shared_ptr<iostream>(new basic_httpbody_chunked_stream<char>(sock));
  else if (header_["Content-Length"] != "")
    stream = pfi::lang::shared_ptr<iostream>(new basic_httpbody_stream<char>(sock, lexical_cast<int64_t>(header_["Content-Length"])));
  else
    stream = pfi::lang::shared_ptr<iostream>(new socketstream(sock));
}

response::~response() {}

const pair<int,int>& response::version() const
{
  return version_;
}

int response::code() const
{
  return code_;
}

const string& response::reason() const
{
  return reason_;
}

header& response::head()
{
  return header_;
}

iostream& response::body()
{
  return *stream;
}

void response::send(const pfi::lang::shared_ptr<stream_socket>& sock)
{
  stringstream* ss = dynamic_cast<stringstream*>(stream.get());
  if (!ss)
    throw http_exception("body is not stringstream");
  string dat = ss->str();

  string resp_line =
    "HTTP/" + lexical_cast<string>(version_.first) + "." + lexical_cast<string>(version_.second) + " " +
    lexical_cast<string>(code_) + " " +
    reason_ + "\r\n";

  if (!sock->puts(resp_line))
    throw http_exception("cannot send response-line");

  if (!header_.has_key("Content-Length") && dat.length() > 0)
    header_["Content-Length"] = lexical_cast<string>(dat.length());
  header_.send(sock);

  if (dat.length() > 0) {
    if (!sock->puts(dat))
      throw http_exception("cannot send body");
  }
  if (sock->flush() >= 0)
    throw http_exception("flush failed");
  if (dat.length() == 0)
    stream = pfi::lang::shared_ptr<iostream>(new socketstream(sock));
}

} // http
} // network
} // pfi
