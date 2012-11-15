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

#include "connection_def.h"
#include "value.h"
#include "../exception.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace database{
namespace postgresql{

static string convert_query(const string &q)
{
  ostringstream oss;

  int cnt=1;

  for (int i=0;i<(int)q.length();i++){
    if (q[i]=='?') oss<<'$'<<(cnt++);
    else oss<<q[i];
  }

  return oss.str();
}

postgresql_statement::postgresql_statement(const pfi::lang::shared_ptr<postgresql_connection_impl>& conn,
                                           const string &query)
  : conn(conn)
{
  postgresql_result res(PQprepare(conn->get(), "", convert_query(query).c_str(), 0, NULL));

  if (!res.command_ok())
    throw database_error(PQresultErrorMessage(res.get()));
}

postgresql_statement::~postgresql_statement()
{
}

int postgresql_statement::execute(const vector<shared_ptr<sql_value> > &args)
{
  int n=args.size();
  vector<shared_ptr<string> > sparams(n);
  vector<const char *> params(n);
  vector<int> param_lens(n);

  for (int i=0;i<n;i++){
    sparams[i]=sql_to_str(args[i]);
    if (sparams[i]){
      params[i]=sparams[i]->c_str();
      param_lens[i]=sparams[i]->length();
    }
    else{
      params[i]=NULL;
      param_lens[i]=0;
    }
  }

  PGresult *r=PQexecPrepared(conn->get(), "", n, &params[0], &param_lens[0], NULL, 0);
  if (r==NULL)
    throw database_error("PQexecPrepared failed");

  shared_ptr<postgresql_result> rtmp=
    shared_ptr<postgresql_result>(new postgresql_result(r));

  if (!rtmp->command_ok() &&
      !rtmp->tuple_ok())
    throw database_error(PQresultErrorMessage(rtmp->get()));

  res.swap(rtmp);

  // TODO: return modified row count
  return 0;
}

bool postgresql_statement::fetch_row(vector<shared_ptr<sql_value> > &row)
{
  if (!res) return false;
  return res->fetch_row(row);
}

} // postgresql
} // database
} // pfi
