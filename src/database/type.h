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

#ifndef INCLUDE_GUARD_PFI_DATABASE_TYPE_H_
#define INCLUDE_GUARD_PFI_DATABASE_TYPE_H_

#include <string>
#include <limits>

#include "../lang/shared_ptr.h"
#include "../system/time_util.h"
#include "exception.h"

namespace pfi{
namespace database{

enum sql_type{
  SQL_BOOL,
  SQL_CHAR,
  SQL_STRING,
  SQL_INT32,
  SQL_INT64,
  SQL_FLOAT,
  SQL_TIMESTAMP,
  SQL_UNKNOWN
};

class sql_value{
public:
  virtual ~sql_value() {}
  virtual sql_type type() const = 0;
};

class sql_bool : public sql_value{
public:
  sql_bool(bool b): dat(b) {}
  sql_type type() const { return SQL_BOOL; }
  bool get() const { return dat; }

private:
  bool dat;
};

class sql_string : public sql_value{
public:
  sql_string(const std::string &s): dat(s) {}
  sql_type type() const { return SQL_STRING; }
  const std::string &get() const { return dat; }

private:
  std::string dat;
};

class sql_int32 : public sql_value{
public:
  sql_int32(int32_t n): dat(n) {}
  sql_type type() const { return SQL_INT32; }
  int32_t get() const { return dat; }

private:
  int32_t dat;
};

class sql_int64 : public sql_value{
public:
  sql_int64(int64_t n): dat(n) {}
  sql_type type() const { return SQL_INT64; }
  int64_t get() const { return dat; }
private:
  int64_t dat;
};

class sql_float : public sql_value{
public:
  sql_float(double d): dat(d) {}
  sql_type type() const { return SQL_FLOAT; }
  double get() const { return dat; }

private:
  double dat;
};

class sql_timestamp : public sql_value{
public:
  sql_timestamp(const pfi::system::time::clock_time &ct): dat(ct) {}
  sql_type type() const { return SQL_TIMESTAMP; }
  const pfi::system::time::clock_time &get() const { return dat; }
private:
  pfi::system::time::clock_time dat;
};

class sql_unknown : public sql_value{
public:
  sql_unknown(const std::string &s): dat(s) {}
  sql_type type() const { return SQL_UNKNOWN; }
  const std::string &get() const { return dat; }

private:
  std::string dat;
};

inline pfi::lang::shared_ptr<sql_value> to_sql(bool b)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_bool(b));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(const std::string &s)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_string(s));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(const char *s)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_string(s));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(int32_t n)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_int32(n));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(int64_t n)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_int64(n));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(double d)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_float(d));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(const pfi::system::time::clock_time &ct)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_timestamp(ct));
}

inline pfi::lang::shared_ptr<sql_value> to_sql(const pfi::system::time::calendar_time &ct)
{
  return pfi::lang::shared_ptr<sql_value>(new sql_timestamp(pfi::system::time::clock_time(ct)));
}

template <class T>
T from_sql(const pfi::lang::shared_ptr<sql_value>& v);

template <>
inline bool from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  sql_bool *p=dynamic_cast<sql_bool*>(v.get());
  if (!p) throw database_error("from_sql: cannot convert to bool");
  return p->get();
}

template <>
inline std::string from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  sql_string *p=dynamic_cast<sql_string*>(v.get());
  if (!p) return "";
  return p->get();
}

template <>
inline int32_t from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  {
    sql_int32 *p=dynamic_cast<sql_int32*>(v.get());
    if (p) return p->get();
  }

  {
    sql_int64 *p=dynamic_cast<sql_int64*>(v.get());
    if (p){
      if (p->get()>=std::numeric_limits<int32_t>::min() &&
          p->get()<=std::numeric_limits<int32_t>::max())
        return static_cast<int32_t>(p->get());
      
      throw database_error("from_sql: cannot convert to int64 to int32, overflow");
    }
  }

  throw database_error("from_sql: cannot convert to int32");
  return 0;
}

template <>
inline int64_t from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  {
    sql_int64 *p=dynamic_cast<sql_int64*>(v.get());
    if (p) return p->get();
  }

  {
    sql_int32 *p=dynamic_cast<sql_int32*>(v.get());
    if (p) return p->get();  
  }

  throw database_error("from_sql: cannot convert to int64");
}

template <>
inline double from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  sql_float *p=dynamic_cast<sql_float*>(v.get());
  if (!p) throw database_error("from_sql: cannot convert to float");
  return p->get();
}

template <>
inline pfi::system::time::clock_time from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  sql_timestamp *p=dynamic_cast<sql_timestamp*>(v.get());
  if (!p) throw database_error("from_sql: cannot convert to clock_time");
  return p->get();
}

template <>
inline pfi::system::time::calendar_time from_sql(const pfi::lang::shared_ptr<sql_value>& v)
{
  sql_timestamp *p=dynamic_cast<sql_timestamp*>(v.get());
  if (!p) throw database_error("from_sql: cannot convert to clock_time");
  return pfi::system::time::calendar_time(p->get());
}

} // database
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATABASE_TYPE_H_
