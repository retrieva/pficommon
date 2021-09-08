#include <gtest/gtest.h>
#include "socket.h"

#include "../lang/bind.h"
#include "../concurrent/thread.h"
#include "../concurrent/mvar.h"

using namespace pfi::network;

struct server_info {
  server_info() {}

  pfi::concurrent::mvar<bool> is_accepted;
  pfi::concurrent::mvar<std::string> first_line;
  pfi::concurrent::mvar<size_t> send_size;
  pfi::concurrent::mvar<size_t> read_size;
};

static void server_thread(pfi::lang::shared_ptr<server_socket> server, server_info* server_info) {
  pfi::lang::shared_ptr<stream_socket> sock(server->accept());

  sock->set_timeout(10);
  server_info->is_accepted.put(true);

  // read line
  std::string line;
  sock->getline(line);
  server_info->first_line.put(line);

  // read buffer
  std::vector<char> buffer(1024);
  size_t send_size = server_info->send_size.take();
  size_t read_size = 0;
  while (read_size < send_size && sock->is_connected()) {
    size_t r = sock->read(buffer.data(), buffer.size());
    read_size += r;
    if (r < buffer.size()) break;  // reach EOF
  }
  server_info->read_size.put(read_size);
}

TEST(socket_server_client, server_test) {
  pfi::lang::shared_ptr<server_socket> server(new server_socket());
  server->set_timeout(10);
  EXPECT_TRUE(server->create(0));
  int port = server->port();
  EXPECT_LT(1024, port);  // ephemeral port

  server_info server_info;
  pfi::concurrent::thread t(pfi::lang::bind(&server_thread, server, &server_info));
  t.start();

  stream_socket ss;
  EXPECT_TRUE(ss.connect("127.0.0.1", port));

  // server accepted
  EXPECT_TRUE(server_info.is_accepted.take());
  EXPECT_TRUE(ss.is_connected());

  EXPECT_EQ("127.0.0.1", ss.remote_addr().to_string());
  EXPECT_EQ(port, ss.remote_port());

  EXPECT_TRUE(ss.puts("hello\n"));
  EXPECT_EQ(-1, ss.flush());

  // wait server read
  EXPECT_EQ("hello", server_info.first_line.take());

  size_t send_size = 2ul * 1024 * 1024 * 1024 + 42;  // > sizeof(int32)
  server_info.send_size.put(send_size);

  std::vector<char> huge_buffer(send_size);
  EXPECT_EQ(send_size, ss.write(huge_buffer.data(), huge_buffer.size()));

  // wait server read
  EXPECT_EQ(send_size, server_info.read_size.take());

  ss.shutdown();

  t.join();
}
