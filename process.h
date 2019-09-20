#ifndef PROCESS_H_
#define PROCESS_H_

#include <array>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>

namespace ttyrex {
namespace process {

class Pipe {
 public:
  Pipe();
  Pipe(const Pipe &) = delete;
  Pipe(Pipe &&) = default;
  ~Pipe();

  void CloseRead();
  void CloseWrite();

  int read_fd() const { return fd_[0]; }
  int write_fd() const { return fd_[1]; }

 private:
  std::array<int, 2> fd_;
};

struct ProcessIO {
  ProcessIO();
  ProcessIO(const ProcessIO &) = delete;
  ProcessIO(ProcessIO &&) = default;
  Pipe in;
  Pipe out;
  Pipe err;
};

// TODO(phil): Do I need this if I have PseudoTerminal instead?
class ChildProcess {
 public:
  ChildProcess();
  ChildProcess(const ChildProcess &) = delete;
  ChildProcess(ChildProcess &&) = default;
  ~ChildProcess();

  // TODO(phil): Return an error when something goes wrong.
  // TODO(phil): Pass in extra environment variables.
  ProcessIO Run(const std::string &path, const std::vector<std::string> &argv);

  void Wait();

 private:
  pid_t pid_ = -1;
};

}  // namespace process
}  // namespace ttyrex

#endif  // PROCESS_H_
