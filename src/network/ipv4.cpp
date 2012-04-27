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

#include "ipv4.h"

#include <sstream>

using namespace std;

namespace pfi{
namespace network{

ipv4_address::ipv4_address()
{
  ip[0]=0;
  ip[1]=0;
  ip[2]=0;
  ip[3]=0;
}

ipv4_address::ipv4_address(const ipv4_address &p)
{
  ip[0]=p.ip[0];
  ip[1]=p.ip[1];
  ip[2]=p.ip[2];
  ip[3]=p.ip[3];
}

ipv4_address::ipv4_address(unsigned char a,
    unsigned char b,
    unsigned char c,
    unsigned char d)
{
  ip[0]=a;
  ip[1]=b;
  ip[2]=c;
  ip[3]=d;
}

ipv4_address::ipv4_address(const string &s)
{
  int a=-1,b=-1,c=-1,d=-1;
  char x='\0',y='\0',z='\0';
  istringstream iss(s);
  if (!((iss>>a>>x>>b>>y>>c>>z>>d)&&
        a>=0&&a<=255&&
        b>=0&&b<=255&&
        c>=0&&c<=255&&
        d>=0&&d<=255&&
        x=='.'&&
        y=='.'&&
        z=='.')){
    *this=none;
    return;
  }
  ip[0]=(unsigned char)a;
  ip[1]=(unsigned char)b;
  ip[2]=(unsigned char)c;
  ip[3]=(unsigned char)d;
}

ipv4_address::~ipv4_address()
{
}

bool ipv4_address::operator==(const ipv4_address &p) const
{
  return
    ip[0]==p.ip[0]&&
    ip[1]==p.ip[1]&&
    ip[2]==p.ip[2]&&
    ip[3]==p.ip[3];
}

bool ipv4_address::operator!=(const ipv4_address &p) const
{
  return !(*this==p);
}

bool ipv4_address::operator<(const ipv4_address &p) const
{
  if (ip[0]!=p.ip[0]) return ip[0]<p.ip[0];
  if (ip[1]!=p.ip[1]) return ip[1]<p.ip[1];
  if (ip[2]!=p.ip[2]) return ip[2]<p.ip[2];
  return ip[3]<p.ip[3];
}

const string ipv4_address::to_string() const
{
  ostringstream oss;
  oss<<(int)ip[0]<<'.'<<(int)ip[1]<<'.'<<(int)ip[2]<<'.'<<(int)ip[3];
  return oss.str();
};

const ipv4_address ipv4_address::any       = ipv4_address(  0,  0,  0,  0);
const ipv4_address ipv4_address::broadcast = ipv4_address(255,255,255,255);
const ipv4_address ipv4_address::loopback  = ipv4_address(127,  0,  0,  1);
const ipv4_address ipv4_address::none      = ipv4_address(255,255,255,255);

} // network
} // pfi
