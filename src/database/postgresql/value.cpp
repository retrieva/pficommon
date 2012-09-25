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

#include "value.h"

#include <iomanip>
#include <iostream>
#include <cstring>

#include "types.h"
#include "../../lang/cast.h"
#include "../../system/time_util.h"

using namespace std;
using namespace pfi::lang;
using namespace pfi::system::time;

namespace pfi{
namespace database{
namespace postgresql{

sql_type oid_to_sql_type(Oid oid)
{
  switch(oid){
  case PG_TYPE_BOOL: return SQL_BOOL;

  case PG_TYPE_INT8: return SQL_INT64;

  case PG_TYPE_INT2:
  case PG_TYPE_INT4:
    return SQL_INT32;

  case PG_TYPE_TEXT:
  case PG_TYPE_VARCHAR:
    return SQL_STRING;

  case PG_TYPE_FLOAT4:
  case PG_TYPE_FLOAT8:
    return SQL_FLOAT;

  case PG_TYPE_TIMESTAMP:
    return SQL_TIMESTAMP;
  }
  cout<<oid<<endl;
  return SQL_UNKNOWN;
}

sql_value *make_sql_value(char *p, int len, sql_type t)
{
  switch(t){
  case SQL_BOOL:{
    bool b;
    if (len==0) b=false;
    else if (p[0]=='t') b=true;
    else if (p[0]=='f') b=false;
    else if (p[0]=='T') b=true;
    else if (p[0]=='y') b=true;
    else if (p[0]=='Y') b=true;
    else if (p[0]=='1') b=true;
    else b=false;
    return new sql_bool(b);
  }

  case SQL_STRING:
    return new sql_string(string(p, p+len));

  case SQL_INT32:
    return new sql_int32(lexical_cast<int32_t>(p));

  case SQL_INT64:
    return new sql_int64(lexical_cast<int64_t>(p));

  case SQL_FLOAT:
    return new sql_float(lexical_cast<double>(p));

  case SQL_TIMESTAMP: {
    tm t;
    char *q=strptime(p, "%Y-%m-%d %T.", &t);
    int usec=atoi(q);
    calendar_time ct(t.tm_year+1900, t.tm_mon+1, t.tm_mday,
                     t.tm_hour, t.tm_min, t.tm_sec, usec);
    return new sql_timestamp(clock_time(ct));
  }

  default:
    return new sql_unknown(string(p, p+len));
  }
}

shared_ptr<sql_value> str_to_sql(char *p, int len, int is_null, Oid oid)
{
  if (is_null) return shared_ptr<sql_value>();
  sql_type t=oid_to_sql_type(oid);
  return shared_ptr<sql_value>(make_sql_value(p, len, t));
}

shared_ptr<string> sql_to_str(const pfi::lang::shared_ptr<sql_value>& p)
{
  if (!p) return shared_ptr<string>();

  switch(p->type()){
  case SQL_BOOL:{
    static const string t("true");
    static const string f("false");
    return shared_ptr<string>(new string(dynamic_cast<sql_bool*>(p.get())->get()?t:f));
  }
  case SQL_STRING:{
    return shared_ptr<string>(new string(dynamic_cast<sql_string*>(p.get())->get()));
  }
  case SQL_INT32:{
    return shared_ptr<string>(new string(lexical_cast<string>(dynamic_cast<sql_int32*>(p.get())->get())));
  }
  case SQL_INT64:{
    return shared_ptr<string>(new string(lexical_cast<string>(dynamic_cast<sql_int64*>(p.get())->get())));
  }
  case SQL_FLOAT:{
    ostringstream oss;
    oss<<setiosflags(ios::fixed)<<setprecision(10)<<dynamic_cast<sql_float*>(p.get())->get();
    return shared_ptr<string>(new string(oss.str()));
  }
  case SQL_TIMESTAMP:{
    calendar_time ct(dynamic_cast<sql_timestamp*>(p.get())->get());
    tm t={};
    t.tm_year=ct.year-1900;
    t.tm_mon=ct.mon-1;
    t.tm_mday=ct.mday;
    t.tm_hour=ct.hour;
    t.tm_min=ct.min;
    t.tm_sec=ct.sec;
    char buf[64], ubuf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d %T.", &t);
    sprintf(ubuf, "%06d", ct.usec);
    strcat(buf, ubuf);
    return shared_ptr<string>(new string(buf));
  }
  case SQL_UNKNOWN:{
    return shared_ptr<string>(new string(dynamic_cast<sql_unknown*>(p.get())->get()));
  }
  default:
    abort();
  }
}

} // postgresql
} // database
} // pfi
