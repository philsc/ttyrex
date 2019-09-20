#include <gtest/gtest.h>

#include "epoll.h"
#include "process.h"

namespace ttyrex {
namespace epoll {
namespace testing {

TEST(EPoll, SimpleTest) {
  EPoll epoll;
  process::ChildProcess child;
  process::ProcessIO io = child.Run("/bin/sleep", {"sleep", "5"});

  bool handler_called = false;

  epoll.Add(io.out.read_fd(), EPOLLIN | EPOLLHUP, [&io, &handler_called] {
    char buffer[1024];
    int result = read(io.out.read_fd(), buffer, sizeof(buffer));
    ASSERT_NE(result, -1);
    ASSERT_EQ(result, 0);
    handler_called = true;
  });

  EXPECT_FALSE(handler_called);
  epoll.Wait(std::chrono::seconds(1));
  EXPECT_FALSE(handler_called);
  epoll.Wait(std::chrono::seconds(100));
  EXPECT_TRUE(handler_called);

  epoll.Delete(io.out.read_fd());
  child.Wait();
}

}  // namespace testing
}  // namespace epoll
}  // namespace ttyrex
