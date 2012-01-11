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

#ifndef INCLUDE_GUARD_PFI_DATABASE_CONNECTION_H_
#define INCLUDE_GUARD_PFI_DATABASE_CONNECTION_H_

#include <string>

#include "statement.h"
#include "type.h"

namespace pfi{
namespace database{

class connection{
public:
  virtual ~connection(){}

  virtual void begin_transaction()=0;
  virtual void commit()=0;
  virtual void rollback()=0;

  virtual int run(const std::string &sql, const std::vector<pfi::lang::shared_ptr<sql_value> > &args)=0;
  virtual statement *prepare(const std::string &sql)=0;

  int run(const std::string &sql){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(0);
    return run(sql, args);
  }

  template <class T1>
  int run(const std::string &sql, T1 a1){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(1);
    args[0]=to_sql(a1);
    return run(sql, args);
  }

  template <class T1, class T2>
  int run(const std::string &sql, T1 a1, T2 a2){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(2);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    return run(sql, args);
  }

  template <class T1, class T2, class T3>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(3);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(4);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(5);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(6);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(7);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(8);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    args[7]=to_sql(a8);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(9);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    args[7]=to_sql(a8);
    args[8]=to_sql(a9);
    return run(sql, args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
  int run(const std::string &sql, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9, T10 a10){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(10);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    args[7]=to_sql(a8);
    args[8]=to_sql(a9);
    args[9]=to_sql(a10);
    return run(sql, args);
  }
};

} // database
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATABASE_CONNECTION_H_
