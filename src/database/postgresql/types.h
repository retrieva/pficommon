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

#ifndef INCLUDE_GUARD_PFI_DATABASE_POSTGRESQL_TYPES_H_
#define INCLUDE_GUARD_PFI_DATABASE_POSTGRESQL_TYPES_H_

namespace pfi{
namespace database{
namespace postgresql{

const int PG_TYPE_BOOL  = 16;
const int PG_TYPE_BYTEA = 17;
const int PG_TYPE_CHAR  = 18;
const int PG_TYPE_NAME  = 19;
const int PG_TYPE_INT8  = 20;
const int PG_TYPE_INT2  = 21;
const int PG_TYPE_INT2VECTOR = 22;
const int PG_TYPE_INT4  = 23;
const int PG_TYPE_REGPROC = 24;
const int PG_TYPE_TEXT  = 25;
const int PG_TYPE_OID   = 26;
const int PG_TYPE_TID   = 27;
const int PG_TYPE_XID   = 28;
const int PG_TYPE_CID   = 29;
const int PG_TYPE_OIDVECTOR = 30;

const int PG_TYPE_POINT   = 600;
const int PG_TYPE_LSEG    = 601;
const int PG_TYPE_PATH    = 602;
const int PG_TYPE_BOX     = 603;
const int PG_TYPE_POLYGON = 604;
const int PG_TYPE_LINE    = 628;

const int PG_TYPE_FLOAT4  = 700;
const int PG_TYPE_FLOAT8  = 701;
const int PG_TYPE_ABSTIME = 702;
const int PG_TYPE_RELTIME = 703;
const int PG_TYPE_TINTERVAL = 704;
const int PG_TYPE_UNKNOWN = 705;

const int PG_TYPE_CIRCLE  = 718;
const int PG_TYPE_MONEY   = 790;
const int PG_TYPE_MACADDR = 829;
const int PG_TYPE_INET    = 869;
const int PG_TYPE_CIDR    = 650;

const int PG_TYPE_BPCHAR    = 1042;
const int PG_TYPE_VARCHAR   = 1043;
const int PG_TYPE_DATE      = 1082;
const int PG_TYPE_TIME      = 1083;
const int PG_TYPE_TIMESTAMP = 1114;
const int PG_TYPE_TIMETZ    = 1266;

const int PG_TYPE_BIT = 1560;

} // postgresql
} // database
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATABASE_POSTGRESQL_TYPES_H_
