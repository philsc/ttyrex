#include "epoll.h"

#include <array>

#include <glog/logging.h>

namespace ttyrex {
namespace epoll {

EPoll::EPoll() : fd_(epoll_create1(EPOLL_CLOEXEC)) { PCHECK(fd_ != -1); }

EPoll::~EPoll() {
  for (const auto &element : functions_) {
    LOG(ERROR) << "Need to delete fd " << element.first
               << " before destruction.";
  }
  CHECK(functions_.empty()) << ": Failed to delete descriptors.";
  PCHECK(close(fd_) == 0);
}

void EPoll::Add(int fd, uint32_t events, std::function<void()> fn) {
  CHECK(functions_.find(fd) == functions_.end())
      << ": Cannot monitor fd " << fd << " twice.";
  struct epoll_event event;
  event.events = events;
  event.data.fd = fd;
  epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &event);
  functions_[fd] = std::move(fn);
}

void EPoll::Delete(int fd) {
  auto it = functions_.find(fd);
  CHECK(it != functions_.end()) << ": fd " << fd << " was never added.";
  functions_.erase(it);
}

void EPoll::Wait(std::chrono::milliseconds timeout) {
  std::array<struct epoll_event, 10> events;
  int result = epoll_wait(fd_, events.data(), events.size(), timeout.count());
  if (result == -1) {
    PCHECK(errno == EINTR) << ": epoll_wait() failed";
    return;
  }

  for (int i = 0; i < result; ++i) {
    const struct epoll_event &event = events[i];
    int fd = event.data.fd;
    auto it = functions_.find(fd);
    CHECK(it != functions_.end()) << ": Encountered unknown fd " << fd;
    it->second();
  }
}

}  // namespace epoll
}  // namespace ttyrex
