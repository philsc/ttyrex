#ifndef EPOLL_H_
#define EPOLL_H_

#include <functional>
#include <sys/epoll.h>
#include <unordered_map>
#include <chrono>

namespace ttyrex {
namespace epoll {

class EPoll {
 public:
  EPoll();
  ~EPoll();

  void Add(int fd, uint32_t events, std::function<void()> fn);

  void Delete(int fd);

  void Wait(std::chrono::milliseconds timeout);

 private:
  const int fd_;
  std::unordered_map<int, std::function<void()>> functions_;
};

}  // namespace epoll
}  // namespace ttyrex

#endif  // EPOLL_H_
