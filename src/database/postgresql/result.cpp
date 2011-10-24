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

#include "result.h"

#include "types.h"
#include "value.h"
#include "../type.h"
#include "../../lang/cast.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace database{
namespace postgresql{

postgresql_result::postgresql_result(PGresult *res)
  : res(res)
  , cur_row(0)
  , row_num(0)
{
  if (tuple_ok()){
    int field_num=PQnfields(res);
    for (int i=0;i<field_num;i++){
      fields.push_back(PQfname(res, i));
      types.push_back(PQftype(res, i));
    }
    row_num=PQntuples(res);
  }
}

postgresql_result::~postgresql_result()
{
  PQclear(res);
}

bool postgresql_result::command_ok()
{
  return PQresultStatus(res)==PGRES_COMMAND_OK;
}

bool postgresql_result::tuple_ok()
{
  return PQresultStatus(res)==PGRES_TUPLES_OK;
}

bool postgresql_result::fetch_row(vector<shared_ptr<sql_value> > &row)
{
  if (cur_row>=row_num)
    return false;

  row.resize(types.size());

  for (int i=0;i<(int)types.size();i++){
    char *p=PQgetvalue(res, cur_row, i);
    int is_null=PQgetisnull(res, cur_row, i);
    int len=PQgetlength(res, cur_row, i);
    row[i]=str_to_sql(p, len, is_null, types[i]);
  }

  cur_row++;
  return true;
}

} // postgresql
} // database
} // pfi
