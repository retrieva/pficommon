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

#include <gtest/gtest.h>

#include "mprpc.h"

#include <vector>
#include <map>

#include "../lang/bind.h"
#include "../concurrent/thread.h"
#include "../system/time_util.h"

using namespace std;
using namespace pfi::lang;
using namespace pfi::concurrent;
using namespace pfi::system::time;

MPRPC_PROC(test_str, string(string));
static string test_str(const string& v){ return v; }
MPRPC_PROC(test_pair, pair<int,string>(pair<int,string>));
static pair<int,string> test_pair(const pair<int,string>& v){ return v; }
MPRPC_PROC(test_vec, vector<int>(vector<int>));
static vector<int> test_vec(const vector<int>& v){ return v; }
MPRPC_PROC(test_map, map<int,int>(map<int,int>));
static map<int, int> test_map(const map<int, int>& v){ return v; }
MPRPC_PROC(test_set, set<int>(set<int> v));
static set<int> test_set(const set<int>& v){ return v; }

MPRPC_GEN(1, testrpc, test_str, test_pair, test_vec, test_map, test_set);

namespace {
const string kLocalhost = "localhost";
const int kServThreads = 10;
const int kTestRPCPort = 31241;
const int kTestStructRPCPort = 31242;
const double kServerTimeout = 3.0;
const double kClientTimeout = 3.0;
const double kTestTimeout = 0.5;
}

static void server_thread(testrpc_server *ser)
{
  ser->set_test_str(&test_str);
  ser->set_test_pair(&test_pair);
  ser->set_test_vec(&test_vec);
  ser->set_test_map(&test_map);
  ser->set_test_set(&test_set);
  ser->serv(kTestRPCPort, kServThreads);
}

TEST(mprpc, mprpc_test)
{
  testrpc_server ser(kServerTimeout);
  thread t(pfi::lang::bind(&server_thread, &ser));
  t.start();

  sleep(1);

  int times = 100;
  EXPECT_NO_THROW({ testrpc_client cln1(kLocalhost, kTestRPCPort, kClientTimeout); });
  {
    testrpc_client cln(kLocalhost, kTestRPCPort, kClientTimeout);
    for (int t=0;t<times;t++) {
      {
        string v;
        for (int i=0;i<10;i++)
          v+='0'+(rand()%10);
        string r;
        EXPECT_NO_THROW({ r = cln.call_test_str(v); });
        EXPECT_EQ(r.size(), 10U);
        for (int i=0;i<10;i++)
          EXPECT_EQ(r[i], v[i]);
      }
      {
        string vs;
        for (int i=0;i<10;i++)
          vs+='0'+(rand()%10);
        pair<int, string> v = make_pair(rand(), vs);
        pair<int, string> r;
        EXPECT_NO_THROW({ r = cln.call_test_pair(v); });
        EXPECT_EQ(r.first, v.first);
        EXPECT_EQ(r.second, v.second);
      }
      {
        vector<int> v;
        for (int i=0;i<10;i++)
          v.push_back(rand());
        vector<int> r;
        EXPECT_NO_THROW({ r = cln.call_test_vec(v); });
        EXPECT_EQ(r.size(), 10U);
        for (int i=0;i<10;i++)
          EXPECT_EQ(r[i], v[i]);
      }
      {
        map<int, int> v;
        for (int i=0;i<10;i++)
          v[i]=rand();
        map<int, int> r;
        EXPECT_NO_THROW({ r = cln.call_test_map(v); });
        EXPECT_EQ(r.size(), 10U);
        for (int i=0;i<10;i++){
          EXPECT_EQ(r[i], v[i]);
        }
      }
      {
        set<int> v;
        for (int i=0;i<10;i++)
          v.insert(i*100);
        set<int> r;
        EXPECT_NO_THROW({ r = cln.call_test_set(v); });
        EXPECT_EQ(r.size(), 10U);
        int cnt = 0;
        for (set<int>::iterator it=v.begin();it!=v.end();++it){
          EXPECT_EQ(*it, cnt++ * 100);
        }
      }
    }
  }

  ser.stop();
  t.join();
}

TEST(mprpc, mprpc_nonblock_serv_test)
{
  testrpc_server ser(kTestTimeout);
  ASSERT_TRUE(ser.create(kTestRPCPort));

  ser.set_test_str(&test_str);
  ser.run(kServThreads, false);
  EXPECT_TRUE(ser.running());

  testrpc_client cln(kLocalhost, kTestRPCPort, kClientTimeout);
  string v, r;
  EXPECT_NO_THROW({ r = cln.call_test_str(v); });

  ser.stop();
  ser.join();
}

TEST(mprpc, mprpc_nonblock_uninitialied_test)
{
  testrpc_server ser(kTestTimeout);
  ASSERT_FALSE(ser.run(kServThreads));
}

TEST(mprpc, mprpc_server_timeout_test)
{
  testrpc_server ser(kTestTimeout);
  thread t(pfi::lang::bind(&server_thread, &ser));
  t.start();

  sleep(1);

  // connect server but server disconnect by timeout
  pfi::network::mprpc::socket sock;
  ASSERT_TRUE(sock.connect(kLocalhost, kTestRPCPort));

  clock_time start_time = get_clock_time();
  int res;
  EXPECT_EQ(0, ::read(sock.get(), &res, sizeof(res)));
  EXPECT_GT((get_clock_time() - start_time), kTestTimeout);

  ser.stop();
  t.join();
}

namespace {
void timeout_server_thread(pfi::network::mprpc::socket *server_socket)
{
  sleep(1);

  ::accept(server_socket->get(), NULL, NULL);
  sleep(1 + kTestTimeout);

  // wait for socket shutdown listened socket
  ::accept(server_socket->get(), NULL, NULL);
}
} // namespace

TEST(mprpc, mprpc_client_timeout_test)
{
  pfi::network::mprpc::socket server_socket;
  server_socket.listen(kTestRPCPort);
  thread t(pfi::lang::bind(&timeout_server_thread, &server_socket));
  t.start();

  sleep(1);

  EXPECT_NO_THROW({ testrpc_client cln1(kLocalhost, kTestRPCPort, kTestTimeout); });

  { // connect server but client disconnect by timeout
    testrpc_client cln(kLocalhost, kTestRPCPort, kTestTimeout);
    string v, r;
    EXPECT_THROW({ r = cln.call_test_str(v); }, pfi::network::mprpc::rpc_timeout_error);
  }

  server_socket.close();
  t.join();
}

// test for struct and empty vector
struct tstruct {
  int i;
  vector<unsigned char> v;

  MSGPACK_DEFINE(i, v);
};

MPRPC_PROC(test_struct, tstruct(tstruct));
MPRPC_GEN(1, test_struct_rpc, test_struct);
static tstruct f_test_struct(tstruct t) { return t; }
static void struct_server_thread(test_struct_rpc_server *ser)
{
  ser->set_test_struct(&f_test_struct);
  ser->serv(kTestStructRPCPort, kServThreads);
}

TEST(mprpc, test_struct)
{
  test_struct_rpc_server ser(kServerTimeout);
  thread t(pfi::lang::bind(&struct_server_thread, &ser));
  t.start();

  sleep(1);

  {
    tstruct t1;
    t1.i = 9;
    EXPECT_NO_THROW({ test_struct_rpc_client cln1(kLocalhost, kTestStructRPCPort, kClientTimeout); });
    test_struct_rpc_client cln(kLocalhost, kTestStructRPCPort, kClientTimeout);

    tstruct t2;
    EXPECT_NO_THROW({ t2 = cln.call_test_struct(t1); });
    EXPECT_EQ(t1.i, t2.i);
  }

  ser.stop();
  t.join();
}
