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

#ifndef INCLUDE_GUARD_PFI_DATABASE_STATEMENT_H_
#define INCLUDE_GUARD_PFI_DATABASE_STATEMENT_H_

#include <vector>
#include <iostream>

#include "type.h"
#include "../lang/shared_ptr.h"

namespace pfi{
namespace database{

class statement{
public:
  virtual ~statement(){}

  virtual int execute(const std::vector<pfi::lang::shared_ptr<sql_value> > &args)=0;
  virtual bool fetch_row(std::vector<pfi::lang::shared_ptr<sql_value> > &args)=0;

  int execute(){
    std::vector<pfi::lang::shared_ptr<sql_value> > args;
    return execute(args);
  }

  template <class T1>
  int execute(T1 a1){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(1);
    args[0]=to_sql(a1);
    return execute(args);
  }

  template <class T1, class T2>
  int execute(T1 a1, T2 a2){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(2);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    return execute(args);
  }

  template <class T1, class T2, class T3>
  int execute(T1 a1, T2 a2, T3 a3){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(3);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(4);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(5);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(6);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(7);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8){
    std::vector<pfi::lang::shared_ptr<sql_value> > args(8);
    args[0]=to_sql(a1);
    args[1]=to_sql(a2);
    args[2]=to_sql(a3);
    args[3]=to_sql(a4);
    args[4]=to_sql(a5);
    args[5]=to_sql(a6);
    args[6]=to_sql(a7);
    args[7]=to_sql(a8);
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9){
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
    return execute(args);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
  int execute(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9, T10 a10){
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
    args[9]=to_sql(a10);
    return execute(args);
  }

  template <class T1>
  bool fetch_row(T1 &r1){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=1) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    return true;
  }

  template <class T1, class T2>
  bool fetch_row(T1 &r1, T2 &r2){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=2) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    return true;
  }

  template <class T1, class T2, class T3>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=3) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    return true;
  }

  template <class T1, class T2, class T3, class T4>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=4) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    return true;
  }

  template <class T1, class T2, class T3, class T4, class T5>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=5) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    return true;
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5, T6 &r6){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=6) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    r6=from_sql<T6>(ret[5]);
    return true;
 }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5, T6 &r6, T7 &r7){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=7) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    r6=from_sql<T6>(ret[5]);
    r7=from_sql<T7>(ret[6]);
    return true;
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5, T6 &r6, T7 &r7, T8 &r8){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=7) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    r6=from_sql<T6>(ret[5]);
    r7=from_sql<T7>(ret[6]);
    r8=from_sql<T8>(ret[7]);
    return true;
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5, T6 &r6, T7 &r7, T8 &r8, T9 &r9){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=8) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    r6=from_sql<T6>(ret[5]);
    r7=from_sql<T7>(ret[6]);
    r8=from_sql<T8>(ret[7]);
    r9=from_sql<T8>(ret[8]);
    return true;
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
  bool fetch_row(T1 &r1, T2 &r2, T3 &r3, T4 &r4, T5 &r5, T6 &r6, T7 &r7, T8 &r8, T9 &r9, T10 &r10){
    std::vector<pfi::lang::shared_ptr<sql_value> > ret;
    if (!fetch_row(ret)) return false;
    if (ret.size()!=9) throw database_error("fetch_row: argument number mismatch");
    r1=from_sql<T1>(ret[0]);
    r2=from_sql<T2>(ret[1]);
    r3=from_sql<T3>(ret[2]);
    r4=from_sql<T4>(ret[3]);
    r5=from_sql<T5>(ret[4]);
    r6=from_sql<T6>(ret[5]);
    r7=from_sql<T7>(ret[6]);
    r8=from_sql<T8>(ret[7]);
    r9=from_sql<T9>(ret[8]);
    r10=from_sql<T10>(ret[9]);
    return true;
  }
};

} // database
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATABASE_STATEMENT_H_
