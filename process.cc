#include "process.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <glog/logging.h>

namespace ttyrex {
namespace process {

namespace {

void WaitForPid(int pid) {
  CHECK_NE(pid, -1) << ": Can't wait on a non-existent child process.";

  int wstatus;
  int options = 0;
  pid_t result = waitpid(pid, &wstatus, options);
  PCHECK(result != -1) << ": waitpid() failed";
  CHECK_EQ(result, pid);

  if (WIFEXITED(wstatus)) {
    int exit_status = WEXITSTATUS(wstatus);
    if (exit_status) {
      LOG(ERROR) << "Child returned non-zero exit status of " << exit_status;
    } else {
      VLOG(1) << "Child exited with exit status of zero.";
    }
  } else if (WIFSIGNALED(wstatus)) {
    LOG(ERROR) << "Child process was terminated by signal "
               << WTERMSIG(wstatus);
    if (WCOREDUMP(wstatus)) {
      LOG(ERROR) << "It also produced a core dump.";
    }
  }
}

}  // namespace

Pipe::Pipe() { PCHECK(pipe2(fd_.data(), O_CLOEXEC) == 0); }

Pipe::~Pipe() {
  CloseRead();
  CloseWrite();
}

void Pipe::CloseRead() {
  if (fd_[0] != -1) {
    PCHECK(close(fd_[0]) == 0);
    fd_[0] = -1;
  }
}

void Pipe::CloseWrite() {
  if (fd_[1] != -1) {
    PCHECK(close(fd_[1]) == 0);
    fd_[1] = -1;
  }
}

ProcessIO::ProcessIO() {
  for (int fd : {out.read_fd(), err.read_fd()}) {
    PCHECK(fcntl(fd, F_SETFL, O_NONBLOCK) == 0);
  }
}

ChildProcess::ChildProcess() {
  // TODO(phil): Implement this.
}

ChildProcess::~ChildProcess() {
  CHECK_EQ(pid_, -1) << ": Must deal with child process before destruction.";
}

ProcessIO ChildProcess::Run(const std::string &path,
                            const std::vector<std::string> &argv) {
  CHECK_EQ(pid_, -1) << ": Process already running.";

  ProcessIO io;
  if ((pid_ = fork()) == 0) {
    // Hook up stdin, stdout, and stderr.
    PCHECK(dup2(io.in.read_fd(), STDIN_FILENO) == STDIN_FILENO);
    PCHECK(dup2(io.out.write_fd(), STDOUT_FILENO) == STDOUT_FILENO);
    PCHECK(dup2(io.err.write_fd(), STDERR_FILENO) == STDERR_FILENO);

    // Set TERM so that we can have a hope of being functional.
    PCHECK(setenv("TERM", "vt100", 1) == 0);

    auto argv_copy = argv;
    std::vector<char *> argv_pointers;
    for (std::string &argv_element : argv_copy) {
      argv_pointers.push_back(&argv_element[0]);
    }
    argv_pointers.push_back(nullptr);
    execv(path.c_str(), argv_pointers.data());
    PLOG(FATAL) << "Failed to exec the shell";
  } else if (pid_ == -1) {
    PLOG(FATAL) << "Failed to fork()";
  }

  // Close the file descriptors that the child is using.
  io.in.CloseRead();
  io.out.CloseWrite();
  io.err.CloseWrite();

  return io;
}

void ChildProcess::Wait() {
  WaitForPid(pid_);
  pid_ = -1;
}

}  // namespace process
}  // namespace ttyrex
