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

#include "uri.h"

#include <cctype>
#include <sstream>
using namespace std;

namespace pfi{
namespace network{

static int tohex(char c)
{
  if (c>='0'&&c<='9') return c-'0';
  if (c>='A'&&c<='F') return c-'A'+10;
  if (c>='a'&&c<='f') return c-'a'+10;
  return 0;
}

static char tochar(int h)
{
  if (h<10) return '0'+h;
  if (h<16) return 'A'+h-10;
  return '\0';
}

std::string uri_encode(const std::string &s)
{
  ostringstream oss;
  for (int i=0;i<(int)s.length();i++){
    if (isalnum(s[i])||
        s[i]=='-'||
        s[i]=='_'||
        s[i]=='.'||
        s[i]=='~')
      oss<<s[i];
    else if (s[i]==' ')
      oss<<'+';
    else{
      int c=(unsigned char)s[i];
      oss<<'%';
      oss<<tochar(c/16);
      oss<<tochar(c%16);
    }
  }
  return oss.str();
}

std::string uri_decode(const std::string &s)
{
  ostringstream oss;
  for (int i=0;i<(int)s.length();i++){
    if (s[i]=='%' && i+2<(int)s.length()){
      oss<<(char)(tohex(s[i+1])*16+tohex(s[i+2]));
      i+=2;
    }
    else if (s[i]=='+')
      oss<<' ';
    else
      oss<<s[i];
  }
  return oss.str();
}

uri::uri(const string &s)
{
  parse(s);
}

uri::~uri()
{
}

const string &uri::scheme() const
{
  return scheme_;
}

string uri::authority() const
{
  string ret;
  if (userinfo_!="") ret=userinfo_+"@";
  ret+=host_;
  if (port_!="") ret+=":"+port_;
  return ret;
}

const string &uri::userinfo() const
{
  return userinfo_;
}

const string &uri::host() const
{
  return host_;
}

const string &uri::port() const
{
  return port_;
}

const string &uri::path() const
{
  return path_;
}

const string &uri::query() const
{
  return query_;
}

const string &uri::fragment() const
{
  return fragment_;
}

bool uri::parse(const string &s)
{
  const char *p=s.c_str();

  clear();
  if (parse_uri(p))
    return true;
  p=s.c_str();
  clear();
  if (parse_relative_ref(p))
    return true;

  clear();
  return false;
}

void uri::clear()
{
  scheme_="";
  userinfo_="";
  host_="";
  port_="";
  path_="";
  query_="";
  fragment_="";
}

bool uri::parse_uri(const char *&p)
{
  if (!parse_scheme(p))
    return false;

  if (*(p++)!=':')
    return false;

  if (!parse_hier_part(p))
    return false;

  if (*p=='?'){
    p++;
    if (!parse_query(p))
      return false;
  }

  if (*p=='#'){
    p++;
    if (!parse_fragment(p))
      return false;
  }

  if (*p)
    return false;

  return true;
}

bool uri::parse_relative_ref(const char *&p)
{
  if (!parse_relative_part(p))
    return false;

  if (*p=='?'){
    p++;
    if (!parse_query(p))
      return false;
  }

  if (*p=='#'){
    p++;
    if (!parse_fragment(p))
      return false;
  }

  if (*p)
    return false;

  return true;
}

bool uri::parse_hier_part(const char *&p)
{
  const char *org=p;

  if (p[0]=='/'&&p[1]=='/'){
    p+=2;

    if (!parse_authority(p))
      return false;
    if (!parse_path_abempty(p))
      return false;
    return true;
  }

  if (parse_path_absolute(p))
    return true;

  p=org;
  if (parse_path_rootless(p))
    return true;

  p=org;
  if (parse_path_empty(p))
    return true;

  return false;
}

bool uri::parse_relative_part(const char *&p)
{
  const char *org=p;

  if (p[0]=='/'&&p[1]=='/'){
    p+=2;
    if (!parse_authority(p))
      return false;
    if (!parse_path_abempty(p))
      return false;
    return true;
  }

  if (parse_path_absolute(p))
    return true;

  p=org;
  if (parse_path_noscheme(p))
    return true;

  p=org;
  if (parse_path_empty(p))
    return true;

  return false;
}

bool uri::parse_scheme(const char *&p)
{
  const char *org=p;
  
  if (!isalpha(*p++))
    return false;

  while(isalnum(*p)||
      *p=='+'||*p=='-'||*p=='.')
    p++;

  scheme_=string(org,p);

  return true;
}

bool uri::parse_authority(const char *&p)
{
  const char *org=p;
  if (parse_userinfo(p) && *p=='@')
    p++;
  else{
    userinfo_="";
    p=org;
  }

  if (!parse_host(p))
    return false;

  if (*p==':'){
    p++;
    if (!parse_port(p))
      return false;
  }

  return true;
}

bool uri::parse_userinfo(const char *&p)
{
  const char *org=p;
  for (;;){
    if (is_unreserved(*p)||
        is_sub_delims(*p)||
        *p==':'){
      p++;
      continue;
    }
    if (parse_pct_encoded(p)){
      p+=3;
      continue;
    }
    break;
  }

  userinfo_=string(org,p);

  return true;
}

bool uri::parse_host(const char *&p)
{
  const char *org=p;

  if (!parse_ip_literal(p)&&
      !(p=org, parse_ipv4address(p))&&
      !(p=org, parse_reg_name(p)))
    return false;

  host_=string(org,p);

  return true;
}

bool uri::parse_port(const char *&p)
{
  const char *org=p;

  while(isdigit(*p))
    p++;

  port_=string(org,p);

  return true;
}

bool uri::parse_ip_literal(const char *&p)
{
  const char *org=p;

  if (*(p++)!='[')
    return false;

  if (!parse_ipv6address(p)){
    p=org+1;
    if (!parse_ipvfuture(p))
      return false;
  }

  if (*(p++)!=']')
    return false;

  host_=string(org,p);

  return true;
}

bool uri::parse_ipvfuture(const char *&p)
{
  if (*(p++)!='v')
    return false;

  if (!isxdigit(*p))
    return false;

  while(isxdigit(*p))
    p++;

  if (*(p++)!='.')
    return false;

  if (!is_unreserved(*p)&&
      !is_sub_delims(*p)&&
      *p!=':')
    return false;

  while(!is_unreserved(*p)&&
      !is_sub_delims(*p)&&
      *p!=':')
    p++;

  return true;
}

bool uri::parse_ipv6address(const char *&p)
{
  const char *org=p;

  if (parse_ipv6_1(p))
    return true;

  if (p=org, parse_ipv6_2(p))
    return true;

  if (p=org, parse_ipv6_3(p))
    return true;

  if (p=org, parse_ipv6_4(p))
    return true;

  if (p=org, parse_ipv6_5(p))
    return true;

  if (p=org, parse_ipv6_6(p))
    return true;

  if (p=org, parse_ipv6_7(p))
    return true;

  if (p=org, parse_ipv6_8(p))
    return true;

  if (p=org, parse_ipv6_9(p))
    return true;

  return false;
}

bool uri::parse_ipv6_1(const char *&p)
{
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_2(const char *&p)
{
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_3(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_4(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_5(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_6(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p) || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_7(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_ls32(p)) return false;
  return true;
}

bool uri::parse_ipv6_8(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  if (!parse_h16(p)) return false;
  return true;
}

bool uri::parse_ipv6_9(const char *&p)
{
  const char *org=p;
  if (!parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p) || *p++!=':' ||
      !parse_h16(p)) p=org;
  if (*p++!=':' || *p++!=':') return false;
  return true;
}

bool uri::parse_h16(const char *&p)
{
  if (!isxdigit(*p++)) return false;
  if (!isxdigit(*p++)) return false; 
  if (!isxdigit(*p++)) return false;
  if (!isxdigit(*p++)) return false;
  return true;
}

bool uri::parse_ls32(const char *&p)
{
  const char *org=p;
  if (parse_ipv4address(p))
    return true;

  p=org;
  if (!parse_h16(p))
    return false;
  if (*p++!=':')
    return false;
  if (!parse_h16(p))
    return false;

  return true;
}

bool uri::parse_ipv4address(const char *&p)
{
  if (!parse_dec_octet(p))
    return false;
  if (*p++!='.')
    return false;
  if (!parse_dec_octet(p))
    return false;
  if (*p++!='.')
    return false;
  if (!parse_dec_octet(p))
    return false;
  if (*p++!='.')
    return false;
  if (!parse_dec_octet(p))
    return false;
  return true;
}

bool uri::parse_dec_octet(const char *&p)
{
  if (p[0]=='2'&&
      p[1]=='5'&&
      p[2]>='0'&&p[2]<='5'){
    p+=3;
    return true;
  }
  if (p[0]=='2'&&
      p[1]>='0'&&p[1]<='4'
      &&isdigit(p[2])){
    p+=3;
    return true;
  }
  if (p[0]=='1'&&
      isdigit(p[1])&&
      isdigit(p[2])){
    p+=3;
    return true;
  }
  if (p[0]>='1'&&p[0]<='9'&&
      isdigit(p[1])){
    p+=2;
    return true;
  }
  if (isdigit(p[0])){
    p++;
    return true;
  }
  return false;
}

bool uri::parse_reg_name(const char *&p)
{
  for (;;){
    if (is_unreserved(*p)){
      p++;
      continue;
    }
    if (is_sub_delims(*p)){
      p++;
      continue;
    }
    const char *org=p;
    if (parse_pct_encoded(p))
      continue;
    p=org;
    break;
  }
  return true;
}

bool uri::parse_path_abempty(const char *&p)
{
  const char *org=p;
  for (;;){
    if (*p!='/')
      break;
    p++;
    if (parse_segment(p))
      continue;
    return false;
  }
  path_=string(org,p);
  return true;
}

bool uri::parse_path_absolute(const char *&p)
{
  const char *org=p;
  if (*p!='/')
    return false;
  p++;

  if (!parse_segment_nz(p)){
    p=org+1;
    path_=string(org,p);
    return true;
  }

  if (!parse_path_abempty(p))
    return false;

  path_=string(org,p);
  return true;
}

bool uri::parse_path_noscheme(const char *&p)
{
  const char *org=p;

  if (!parse_segment_nz_nc(p))
    return false;

  if (!parse_path_abempty(p))
    return false;

  path_=string(org,p);
  return true;
}

bool uri::parse_path_rootless(const char *&p)
{
  const char *org=p;

  if (!parse_segment_nz(p))
    return false;

  if (!parse_path_abempty(p))
    return false;

  path_=string(org,p);
  return true;
}

bool uri::parse_path_empty(const char*&)
{
  path_="";
  return true;
}

bool uri::parse_segment(const char *&p)
{
  while(parse_pchar(p));
  return true;
}

bool uri::parse_segment_nz(const char *&p)
{
  if (!parse_pchar(p))
    return false;
  return parse_segment(p);
}

bool uri::parse_segment_nz_nc(const char *&p)
{
  if (!parse_pchar(p,false))
    return false;
  while(parse_pchar(p,false));
  return true;
}

bool uri::parse_pchar(const char *&p, bool colon)
{
  if (is_unreserved(*p)){
    p++;
    return true;
  }
  if (parse_pct_encoded(p)){
    return true;
  }
  if (is_sub_delims(*p)){
    p++;
    return true;
  }
  if (colon && *p==':'){
    p++;
    return true;
  }
  if (*p=='@'){
    p++;
    return true;
  }
  return false;
}

bool uri::parse_query(const char *&p)
{
  const char *org=p;
  for (;;){
    if (parse_pchar(p))
      continue;
    if (*p=='/'){
      p++;
      continue;
    }
    if (*p=='?'){
      p++;
      continue;
    }
    break;
  }
  query_=string(org,p);
  return true;
}

bool uri::parse_fragment(const char *&p)
{
  const char *org=p;
  for (;;){
    if (parse_pchar(p))
      continue;
    if (*p=='/'){
      p++;
      continue;
    }
    if (*p=='?'){
      p++;
      continue;
    }
    break;
  }
  fragment_=string(org,p);
  return true;
}

bool uri::parse_pct_encoded(const char *&p)
{
  if (p[0]!='%')
    return false;
  if (!isxdigit(p[1]))
    return false;
  if (!isxdigit(p[2]))
    return false;
  p+=3;
  return true;
}

bool uri::is_unreserved(char c)
{
  if (isalpha(c))
    return true;
  if (isdigit(c))
    return true;
  if (c=='-'||
      c=='.'||
      c=='_'||
      c=='~')
    return true;
  return false;
}

bool uri::is_reserved(char c)
{
  return is_gen_delims(c)||is_sub_delims(c);
}

bool uri::is_gen_delims(char c)
{
  return (c==':'||
      c=='/'||
      c=='?'||
      c=='#'||
      c=='['||
      c==']'||
      c=='@');
}

bool uri::is_sub_delims(char c)
{
  return (c=='!'||
      c=='$'||
      c=='&'||
      c=='\''||
      c=='('||
      c==')'||
      c=='*'||
      c=='+'||
      c==','||
      c==';'||
      c=='=');
}

ostream &operator<<(ostream &os, const uri &u)
{
  if (u.scheme()!="")
    os<<u.scheme()<<"://";

  if (u.authority()!="")
    os<<u.authority();

  if (u.path()!="")
    os<<u.path();
  else
    os<<"/";

  if (u.query()!="")
    os<<"?"<<u.query();

  if (u.fragment()!="")
    os<<"#"<<u.fragment();

  return os;
}

} // pfi
} // network
