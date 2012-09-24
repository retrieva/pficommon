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

#include "statement.h"

#include <sstream>
#include <memory.h>

#include <errmsg.h> // for MySQL errno

#include "value.h"
#include "connection_def.h"
#include "../exception.h"
#include "../../lang/cast.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace database{
namespace mysql{

mysql_statement::mysql_statement(pfi::lang::shared_ptr<mysql_connection_impl> &conn,
                                 const string &query)
  :conn(conn),meta(NULL)
{
  stmt=mysql_stmt_init(conn->get());
  if (stmt==NULL)
    throw database_error("cannot create statement");

  int rc=mysql_stmt_prepare(stmt, query.c_str(), query.length());
  if (rc!=0){
    rc=mysql_stmt_errno(stmt);
    string msg(string("cannot prepare statement: E")+
               lexical_cast<string>(rc)+": "+
               mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    if (rc==CR_SERVER_GONE_ERROR||rc==CR_SERVER_LOST)
      throw connection_error(msg);
    else
      throw database_error(msg);
  }

  param_count=mysql_stmt_param_count(stmt);

  meta=mysql_stmt_result_metadata(stmt);
  if (meta==NULL){
    flds.clear();
  }
  else{
    MYSQL_FIELD *fld=mysql_fetch_fields(meta);
    if (fld==NULL){
      mysql_free_result(meta);
      throw database_error("cannot fetch field metadata");
    }
    
    unsigned int fnum=mysql_num_fields(meta);
    flds.clear();
    
    for(unsigned int i=0; i<fnum; i++)
      flds.push_back(fld[i]);
  }
}

mysql_statement::~mysql_statement()
{
  if (meta!=NULL)
    mysql_free_result(meta);
  mysql_stmt_close(stmt);
}

int mysql_statement::execute(const vector<pfi::lang::shared_ptr<sql_value> > &args)
{
  if (param_count!=args.size()){
    ostringstream msg;
    msg<<"parameter number mismatch: "<<param_count<<
      " expected, but got "<<args.size()<<endl;
    throw database_error(msg.str());
  }

  vector<MYSQL_BIND> bind(args.size());

  vector<vector<char> > ibufs(args.size());
  vector<my_bool> falses(args.size(), 0);
  vector<unsigned long> lns(args.size(), 0);

  for (size_t i=0;i<args.size();i++){
    memset(&bind[i], 0, sizeof(MYSQL_BIND));
    ibufs[i].resize(bind_length(args[i]));
    bind[i].buffer=&ibufs[i][0];
    bind[i].buffer_length=ibufs[i].size();
    bind[i].is_null=&falses[i];
    bind[i].length=&lns[i];
    to_bind(args[i], bind[i]);
  }

  my_bool rb=mysql_stmt_bind_param(stmt, &bind[0]);
  if (rb!=0){
    throw database_error(string("cannot bind parameter: ")+
                         mysql_stmt_error(stmt));
  }

  int rc=mysql_stmt_execute(stmt);
  if (rc!=0){
    throw database_error(string("cannot execute statement")+
                         mysql_stmt_error(stmt));
  }

  binds.clear();
  lens.clear();
  bufs.clear();
  nulls.clear();

  // 2009/11/17 Nobuyuki Kubota <nobu@preferred.jp>
  // Calling mysql_stmt_affected_rows(stmt) here always returns -1
  // because mysql_stmt_fetch has not been called.
  return 0;
}

bool mysql_statement::fetch_row(vector<pfi::lang::shared_ptr<sql_value> > &row)
{
  static const size_t INIT_BUF_SIZE = 16*1024;
  
  if (binds.size()==0){
    size_t fn=flds.size();

    binds.resize(fn);
    bufs.resize(fn, vector<char>(INIT_BUF_SIZE));
    lens.resize(fn);
    nulls.resize(fn);

    for (size_t i=0;i<fn;i++){
      memset(&binds[i], 0, sizeof(MYSQL_BIND));
      nulls[i]=0;
      lens[i]=0;

      binds[i].buffer_type=flds[i].type;
      binds[i].buffer=&bufs[i][0];
      binds[i].buffer_length=bufs[i].size();
      binds[i].length=&lens[i];
      binds[i].is_null=&nulls[i];
    }

    mysql_stmt_bind_result(stmt, &binds[0]);
  }

  int rc=mysql_stmt_fetch(stmt);

  if (rc==MYSQL_DATA_TRUNCATED){
    size_t fn=flds.size();
    for (size_t i=0;i<fn;i++){
      if (binds[i].buffer_length<*binds[i].length){
        bufs[i].resize(*binds[i].length+1);
        binds[i].buffer=&bufs[i][0];
        binds[i].buffer_length=bufs[i].size();
        int rr=mysql_stmt_fetch_column(stmt, &binds[i], i, 0);
        if (rr!=0)
          throw database_error(string("mysql_statement::fetch_row(): ")+
                               mysql_stmt_error(stmt));
      }
    }
    mysql_stmt_bind_result(stmt, &binds[0]);
  }

  if (rc==MYSQL_NO_DATA)
    return false;

  if (rc==1)
    throw database_error(string("fetch_row(): ")+
                         mysql_stmt_error(stmt));

  row.resize(flds.size());
  for (size_t i=0;i<flds.size();i++)
    row[i]=from_bind(binds[i]);

  return true;
}

} // postgresql
} // databse
} // pfi
