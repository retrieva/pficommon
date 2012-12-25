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

#include <errmsg.h> // for MySQL errno

#include "connection_def.h"

using namespace std;
using namespace pfi::lang;

namespace pfi{
namespace database{
namespace mysql{

mysql_connection_impl::mysql_connection_impl(const string &host,
                                             const string &user,
                                             const string &passwd,
                                             const string &db,
                                             uint16_t port)
  :conn(NULL), host(host), user(user), passwd(passwd), db(db), port(port)
{
  reconnect();
}

mysql_connection_impl::~mysql_connection_impl()
{
  if (conn){
    mysql_close(conn);
    conn=NULL;
  }
}

void mysql_connection_impl::reconnect()
{
  if (conn){
    if (mysql_ping(conn) == 0) return;
    mysql_close(conn);
    conn=NULL;
  }

  MYSQL *c=mysql_init(NULL);

  if (c==NULL)
    throw database_error("cannot create connection");

  conn=mysql_real_connect(c,
                          host.c_str(),
                          user.c_str(),
                          passwd.c_str(),
                          db.c_str(),
                          port,
                          NULL,
                          0);

  if (conn==NULL){
    string msg(string("cannot connect to db: ")+
               mysql_error(c));
    mysql_close(c);
    throw connection_error(msg);
  }
}

mysql_connection::mysql_connection(const string &host,
                                   const string &user,
                                   const string &passwd,
                                   const string &db,
                                   uint16_t port)
  :conn(new mysql_connection_impl(host, user, passwd, db, port))
{
}

mysql_connection::~mysql_connection()
{
  try{
    (void)query("ROLLBACK");
  }catch(...){
  }
}

void mysql_connection::begin_transaction()
{
  (void)query("BEGIN");
}

void mysql_connection::commit()
{
  (void)query("COMMIT");
}

void mysql_connection::rollback()
{
  (void)query("ROLLBACK");
}

int mysql_connection::query(const string &sql)
{
  if (conn->get() == NULL) reconnect();

  int rc=mysql_query(conn->get(), sql.c_str());
  if (rc == 0) return 0;

  rc = mysql_errno(conn->get());
  if (rc == CR_SERVER_GONE_ERROR ||
      rc == CR_SERVER_LOST) {
    reconnect();
  } else {
    return rc;
  }
  return mysql_query(conn->get(), sql.c_str());
}

void mysql_connection::reconnect()
{
  conn->reconnect();
}

int mysql_connection::run(const string &sql,
                          const vector<pfi::lang::shared_ptr<sql_value> > &args)
{
  if (conn->get() == NULL) reconnect();

  // TODO: optimize
  try {
    pfi::lang::shared_ptr<statement> stat(prepare(sql));
    return stat->execute(args);
  }
  catch (connection_error&) {
    reconnect(); // this function rethrows an exception if necessary
  }

  pfi::lang::shared_ptr<statement> stat(prepare(sql));
  return stat->execute(args);
}

statement *mysql_connection::prepare(const string &sql)
{
  try {
    return new mysql_statement(conn, sql);
  }
  catch (connection_error&) {
    reconnect();
  }
  return new mysql_statement(conn, sql);
}

connection *connect(const std::string &host,
                    uint16_t port,
                    const std::string &dbname,
                    const std::string &user,
                    const std::string &password)
{
  return new mysql_connection(host, user, password, dbname, port);
}

} // mysql
} // database
} // pfi
