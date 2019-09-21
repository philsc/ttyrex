#include <gtest/gtest.h>
#include <glog/logging.h>

#include "pts.h"

namespace ttyrex {
namespace pts {
namespace testing {

TEST(PseudoTerminal, Bash) {
  PseudoTerminal terminal;
  terminal.Fork("/bin/bash", {"bash"});

  uint8_t buffer[1024];

  while (true) {
    int result = read(terminal.master(), buffer, sizeof(buffer));
    if (result == -1) {
      PLOG(ERROR) << "Read failed";
    } else {
      for (int i = 0; i < result; ++i) {
        LOG(INFO) << "Got " << std::hex << std::setw(2) << static_cast<int>(buffer[i]) << " "
                  << (isprint(buffer[i]) ? static_cast<char>(buffer[i]) : ' ');
      }
    }
  }
}

}  // namespace testing
}  // namespace pts
}  // namespace ttyrex
