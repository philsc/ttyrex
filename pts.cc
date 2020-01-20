#include "pts.h"

#include <pty.h>
#include <sys/wait.h>

#include <glog/logging.h>

namespace ttyrex {
namespace pts {

namespace {

// TODO(phil): De-duplidate with what's in process.cc.
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

PseudoTerminal::PseudoTerminal() {
  // TODO(phil): Implement this.
}

PseudoTerminal::~PseudoTerminal() {
  CHECK_NE(master_, -1) << ": Must wait for child before destruction.";
}

void PseudoTerminal::Fork(const std::string &path,
                          const std::vector<std::string> &argv) {
  CHECK_EQ(pid_, -1) << ": Process already running.";

  if ((pid_ = forkpty(&master_, nullptr, nullptr, nullptr)) == 0) {
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
    PLOG(FATAL) << "Failed to open a pseudo terminal.";
  }
}

void PseudoTerminal::Wait() {
  CHECK_NE(pid_, -1);
  WaitForPid(master_);
  master_ = -1;
}

void PseudoTerminal::KillAndWait() {
  CHECK_NE(pid_, -1);
  PCHECK(kill(pid_, SIGTERM) == 0);
  WaitForPid(master_);
  master_ = -1;
}

}  // namespace pts
}  // namespace ttyrex
