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

#include "connection.h"

#include <sstream>
#include <cassert>

#include <libpq-fe.h>

#include "connection_def.h"
#include "statement.h"
#include "../exception.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace database{
namespace postgresql{

postgresql_connection_impl::postgresql_connection_impl(const string &conninfo)
  :conn(NULL)
{
  if (PQisthreadsafe()!=1)
    throw database_error("PostgreSQL is not threadsafe");

  conn=PQconnectdb(conninfo.c_str());

  if (conn==NULL)
    throw database_error("cannot create connection");

  if (PQstatus(conn)!=CONNECTION_OK){
    PQfinish(conn);
    throw database_error("fail to connect database");
  }
}

postgresql_connection_impl::~postgresql_connection_impl()
{
  if (conn){
    PQfinish(conn);
    conn=NULL;
  }
}

void postgresql_connection_impl::reconnect()
{
  if (PQstatus(conn)==CONNECTION_OK)
    return;

  PQreset(conn);

  if (PQstatus(conn)!=CONNECTION_OK)
    throw database_error("fail to reconnect");
}

postgresql_connection::postgresql_connection(const string &conninfo)
  :conn(new postgresql_connection_impl(conninfo))
{
}

postgresql_connection::~postgresql_connection()
{
  (void)connection::run("ROLLBACK");
}

void postgresql_connection::begin_transaction()
{
  (void)connection::run("BEGIN");
}

void postgresql_connection::commit()
{
  (void)connection::run("COMMIT");
}

void postgresql_connection::rollback()
{
  (void)connection::run("ROLLBACK");
}

int postgresql_connection::run(const string &sql, const vector<shared_ptr<sql_value> > &args)
{
  shared_ptr<statement> stat(prepare(sql));
  return stat->execute(args);
}

statement *postgresql_connection::prepare(const string &sql)
{
  return new postgresql_statement(conn, sql);
}

connection *connect(const string &conninfo)
{
  return new postgresql_connection(conninfo);
}

connection *connect(const string &host,
                    uint16_t port,
                    const string &dbname,
                    const string &user,
                    const string &password)
{
  ostringstream oss;
  oss<<"host="<<host<<" "
     <<"port="<<port<<" "
     <<"dbname="<<dbname<<" "
     <<"user="<<user<<" "
     <<"password="<<password;
  return connect(oss.str());
}

} // postgresql
} // database
} // pfi
