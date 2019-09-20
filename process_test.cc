#include <string.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include "process.h"

namespace ttyrex {
namespace process {
namespace testing {

// Validates that we can read output from echo(1).
TEST(ChildProcess, Echo) {
  ChildProcess child;
  ProcessIO io = child.Run("/bin/echo", {"echo", "hi"});
  child.Wait();

  char buffer[1024];
  int result = read(io.out.read_fd(), buffer, sizeof(buffer));
  ASSERT_NE(result, -1) << strerror(errno);
  buffer[result] = 0;
  ASSERT_STREQ(buffer, "hi\n");

  result = read(io.err.read_fd(), buffer, sizeof(buffer));
  ASSERT_EQ(result, 0);
}

// Validates that text sent to cat(1) is retrieved again properly.
TEST(ChildProcess, Cat) {
  ChildProcess child;
  ProcessIO io = child.Run("/bin/cat", {"cat", "-"});

  static constexpr char kTestString[] = "Hello\nWorld\n";
  ASSERT_EQ(write(io.in.write_fd(), kTestString, sizeof(kTestString)),
            sizeof(kTestString));
  io.in.CloseWrite();
  child.Wait();

  char buffer[1024];
  int result = read(io.out.read_fd(), buffer, sizeof(buffer));
  ASSERT_NE(result, -1) << strerror(errno);
  buffer[result] = 0;
  ASSERT_STREQ(buffer, kTestString);

  result = read(io.err.read_fd(), buffer, sizeof(buffer));
  ASSERT_EQ(result, 0);
}

}  // namespace testing
}  // namespace process
}  // namespace ttyrex
