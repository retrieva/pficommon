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

#include <cstring>

#include "tags.h"
#include "../uri.h"
#include "../../lang/cast.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace network{
namespace cgi{

cgi::cgi()
  : max_content_length(4*1024*1024)
{
}

void cgi::set_max_content_length(size_t length)
{
  max_content_length=length;
}

void cgi::exec(istream& in,
               ostream& out,
               ostream& /* err */,
               const map<string, string>& cenv)
{
  env=cenv;
  map<string,string>().swap(query);

  bool valid=true;
  string invalid_msg;

  head=http::header();
  query.clear();
  multi_query.clear();
  file.clear();
  file_info.clear();

  method=env["REQUEST_METHOD"];

  if (method=="GET"){
    parse_input(env["QUERY_STRING"]);
  }
  else if (method=="POST"){
    size_t len=0;
    try{ len=lexical_cast<size_t>(env["CONTENT_LENGTH"]); } catch(...){}
    if (len>max_content_length){
      valid=false;
      ostringstream msg;
      msg<<"Content length is too long: got "<<len<<" bytes, expect less than "<<max_content_length<<" bytes";
      invalid_msg=msg.str();
    }
    else{
      string dat(len, '\0');
      in.read(&dat[0], len);
      if (in.eof()){
        valid=false;
        invalid_msg="Request is too short";
      }
      else{
        if (env["CONTENT_TYPE"].find("multipart/")!=string::npos){
          string::size_type ix=env["CONTENT_TYPE"].find("boundary=");
          if (ix==string::npos){
            valid=false;
            invalid_msg="Invalid html protocol";
          }
          string boundary=env["CONTENT_TYPE"].substr(ix+9);
          parse_multipart_input(dat, boundary);
        }
        else
          parse_input(dat);
      }
    }
  }

  head["Content-Type"]="text/plain; charset=utf-8";

  ostringstream oss;
  if (valid)
    run(oss);
  else{
    head["Status"]="500 Internal Server Error";
    xhtml_builder builder;
    body__{
      head__ title__ text__("Internal Server Error");
      body__{
 	text__(invalid_msg);
      }
    }
    vector<xhtml_builder::elem_type> &rs=builder.get();
    for (int i=0;i<(int)rs.size();i++)
      rs[i]->pretty(oss);
  }

  for (http::header::iterator p=head.begin();
       p!=head.end(); p++)
    out<<p->first<<": "<<p->second<<endl;
  out<<endl;
  
  out<<oss.str();
}

void cgi::parse_input(const string &s)
{
  const char *p=s.c_str();
  
  while(*p){
    const char *q=strchr(p,'&');
    if (!q) q=s.c_str()+s.length();
    
    const char *r=strchr(p,'=');
    if (r && r<q){
      string k = uri_decode(string(p,r));
      string v = uri_decode(string(r+1,q));
      query[k]= v;
      multi_query[k].push_back(v);
    }
    else{
      string k = uri_decode(string(p,q));
      query[k]="";
      multi_query[k].push_back("");
    }
    
    p=*q?q+1:q;
  }
}

static inline string trim(const string &s)
{
  ssize_t st=0, ed=s.length()-1;
  while(st<=ed && isspace(s[st])) st++;
  while(st<=ed && isspace(s[ed])) ed--;
  return s.substr(st, ed-st+1);
}

static inline string string_tolower(const string &s)
{
  string ret=s;
  for (size_t i=0; i<ret.length(); i++)
    ret[i]=tolower(ret[i]);
  return ret;
}

static inline pair<string, string> extract_attr(const string &s)
{
  string::size_type ix=s.find('=');
  string key=string_tolower(trim(s.substr(0, ix)));
  if (ix==string::npos) return make_pair(key, "");
  ix++;
  string::size_type jx=s.find('\"', ix);
  if (jx==string::npos) return make_pair(key, "");
  string::size_type kx=s.find('\"', jx+1);
  if (kx==string::npos) return make_pair(key, "");
  return make_pair(key, s.substr(jx+1, kx-jx-1));
}

static inline pair<string, map<string, string> >
extract_header(const string &s)
{
  string val;
  map<string, string> attr;

  string::size_type cur=s.find(';');
  val=s.substr(0, cur);

  for (; cur!=string::npos; ){
    cur++;
    string::size_type next=s.find(';', cur);
    attr.insert(extract_attr(s.substr(cur, next)));
    cur=next;
  }

  return make_pair(val, attr);
}

void cgi::parse_multipart_input(const string &s, const string &boundary)
{
  bool end=false;

  istringstream iss(s);
  for (string line; getline(iss, line); ){
    if (line.substr(0, boundary.length()+2)=="--"+boundary){
      if (line.substr(0, boundary.length()+4)=="--"+boundary+"--")
        end=true;
      break;
    }
  }

  for (; !end; ){
    string str;
    ssize_t cnt=0;
    for (char a=0, b=0, c=0; iss.get(c); a=b, b=c, cnt++){
      if (a=='\n' && b=='-' && c=='-'){
        string chk(boundary.length(), ' ');
        iss.read(&chk[0], boundary.length());
        if (chk==boundary){
          int cnt=0;
          while(iss.get(a) && a!='\n'){
            if (a=='-'){ if (cnt>=0 && ++cnt==2) end=true; }
            else cnt=-1;
          }
          if (str.length()>0 && str[str.length()-1]=='\r')
            str.resize(str.length()-1);
          break;
        }
        str+=a;
        str+="--";
        str+=chk;
        cnt=-1;
        continue;
      }
      if (cnt>=2) str+=a;
    }

    istringstream jss(str);
    http::header hd(jss);
    string body=(jss?str.substr(jss.tellg()):"");
    const string &ct=hd["Content-Type"];
    if (ct.find("multipart/")!=string::npos){
      string::size_type ix=ct.find("boundary=");
      string bd=ct.substr(ix+9);
      parse_multipart_input(body, bd);
    }
    else{
      const string &cd=hd["Content-Disposition"];
      pair<string, map<string, string> > p=extract_header(cd);

      if (p.first=="form-data" &&
          p.second.count("name")>0 &&
          p.second.count("filename")>0){
        query[p.second["name"]]=p.second["filename"];
        multi_query[p.second["name"]].push_back(p.second["filename"]);
        file[p.second["filename"]]=body;
        file_info[p.second["filename"]]=hd;
      }
      else if (p.first=="form-data" &&
               p.second.count("name")>0){
        query[p.second["name"]]=body;
        multi_query[p.second["name"]].push_back(body);
      }
      else if (p.first=="attachment"){
        file[p.second["filename"]]=body;
        file_info[p.second["filename"]]=hd;
      }
    }
  }
}

} // cgi
} // network
} // pfi
