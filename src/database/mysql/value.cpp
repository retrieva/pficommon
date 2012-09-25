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

#include <cstring>

using namespace std;
using namespace pfi::lang;
using namespace pfi::system::time;

namespace pfi{
namespace database{
namespace mysql{

static sql_value *from_bind_p(MYSQL_BIND &bind)
{
  if (!bind.is_null || *bind.is_null)
    return NULL;

  switch(bind.buffer_type){
  case MYSQL_TYPE_TINY:
    return new sql_int32(*(signed char*)bind.buffer);

  case MYSQL_TYPE_SHORT:
    return new sql_int32(*(short*)bind.buffer);

  case MYSQL_TYPE_LONG:
    return new sql_int32(*(int*)bind.buffer);

  case MYSQL_TYPE_LONGLONG:
    return new sql_int64(*(long long*)bind.buffer);

  case MYSQL_TYPE_FLOAT:
    return new sql_float(*(float*)bind.buffer);

  case MYSQL_TYPE_DOUBLE:
    return new sql_float(*(double*)bind.buffer);

  case MYSQL_TYPE_TIME:
  case MYSQL_TYPE_DATE:
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_TIMESTAMP: {
    MYSQL_TIME *mt=(MYSQL_TIME*)bind.buffer;
    calendar_time ct(mt->year, mt->month, mt->day, mt->hour, mt->minute, mt->second, mt->second_part);
    return new sql_timestamp(clock_time(ct));
  }

  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_BLOB:{
    char *p=(char*)bind.buffer;
    return new sql_string(string(p, p+*bind.length));
  }

  case MYSQL_TYPE_NULL:
    return NULL;

  default:
    throw database_error("unsupported datatype");
  }
}

pfi::lang::shared_ptr<sql_value> from_bind(MYSQL_BIND &bind)
{
  sql_value *p=from_bind_p(bind);
  if (!p) return pfi::lang::shared_ptr<sql_value>();
  return pfi::lang::shared_ptr<sql_value>(p);
}

size_t bind_length(const pfi::lang::shared_ptr<sql_value>& p)
{
  if (!p) return 0;

  switch(p->type()){
  case SQL_BOOL:
    return sizeof(int);
  case SQL_STRING:
    return dynamic_cast<sql_string*>(p.get())->get().length();
  case SQL_INT32:
    return sizeof(int);
  case SQL_INT64:
    return sizeof(long long);
  case SQL_FLOAT:
    return sizeof(double);
  case SQL_TIMESTAMP: {
    return sizeof(MYSQL_TIME);
  }

  default:
    throw database_error("unsupported datatype");
  }
}

void to_bind(const pfi::lang::shared_ptr<sql_value>& p, MYSQL_BIND &bind)
{
  if (!p){
    bind.is_null=NULL;
    return;
  }

  switch(p->type()){
  case SQL_BOOL:
    *(int*)bind.buffer=dynamic_cast<sql_bool*>(p.get())->get()?1:0;
    *bind.length=sizeof(int);
    bind.buffer_type=MYSQL_TYPE_LONG;
    break;

  case SQL_STRING:
    memcpy((char*)bind.buffer,
           dynamic_cast<sql_string*>(p.get())->get().c_str(),
           dynamic_cast<sql_string*>(p.get())->get().length());
    *bind.length=dynamic_cast<sql_string*>(p.get())->get().length();
    bind.buffer_type=MYSQL_TYPE_STRING;
    break;

  case SQL_INT32:
    *(int*)bind.buffer=dynamic_cast<sql_int32*>(p.get())->get();
    *bind.length=sizeof(int);
    bind.buffer_type=MYSQL_TYPE_LONG;
    break;

  case SQL_INT64:
    *(long long*)bind.buffer=dynamic_cast<sql_int64*>(p.get())->get();
    *bind.length=sizeof(long long);
    bind.buffer_type=MYSQL_TYPE_LONGLONG;
    break;

  case SQL_FLOAT:
    *(double*)bind.buffer=dynamic_cast<sql_float*>(p.get())->get();
    *bind.length=sizeof(double);
    bind.buffer_type=MYSQL_TYPE_DOUBLE;
    break;

  case SQL_TIMESTAMP: {
    MYSQL_TIME *mt=reinterpret_cast<MYSQL_TIME*>(bind.buffer);
    calendar_time ct(dynamic_cast<sql_timestamp*>(p.get())->get());
    mt->year=ct.year;
    mt->month=ct.mon;
    mt->day=ct.mday;
    mt->hour=ct.hour;
    mt->minute=ct.min;
    mt->second=ct.sec;
    mt->neg=0;
    mt->second_part=ct.usec;
    *bind.length=sizeof(MYSQL_TIME);
    bind.buffer_type=MYSQL_TYPE_TIMESTAMP;
    break;
  }

  default:
    throw database_error("unsupported datatype");
  }
}

} // mysql
} // database
} // pfi
