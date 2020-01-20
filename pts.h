#ifndef PTS_H_
#define PTS_H_

#include <string>
#include <vector>

namespace ttyrex {
namespace pts {

class PseudoTerminal {
 public:
  PseudoTerminal();
  PseudoTerminal(const PseudoTerminal &) = delete;
  PseudoTerminal(PseudoTerminal &&) = default;
  ~PseudoTerminal();

  void Fork(const std::string &path, const std::vector<std::string> &argv);

  void Wait();
  void KillAndWait();

  int master() const { return master_; }

 private:
  int master_ = -1;
  pid_t pid_ = -1;
};

}  // namespace pts
}  // namespace ttyrex

#endif  // PTS_H_
