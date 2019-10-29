#ifndef PARSER_H_
#define PARSER_H_

#include <iterator>
#include <vector>
#include <span>

#include "epoll.h"
#include "zero_copy.h"

namespace ttyrex::parser {

enum class Colour {
  kBlack,
  kRed,
  kGreen,
  kYellow,
  kBlue,
  kMagenta,
  kCyan,
  kWhite,
  kReset,
};

struct Slot {
  Colour foreground;
  int character;
};

using ParsedRow = std::span<const Slot>;

class ParsedScreen;

class Iterator {
 public:
  using difference_type = int;
  using value_type = ParsedRow;
  using pointer = ParsedRow *;
  using reference = ParsedRow;
  using iterator_category = std::forward_iterator_tag;

  Iterator(int row_index, const ParsedScreen &parsed_screen);
  Iterator(const Iterator &) = default;

  Iterator &operator=(const Iterator &) = default;
  auto operator<=>(const Iterator &other) {
    return row_index_ - other.row_index_;
  }
  Iterator &operator++() {
    ++row_index_;
    return *this;
  }
  ParsedRow operator*() const;

 private:
  int row_index_;
  const ParsedScreen *parsed_screen_;
};

class ParsedScreen {
 public:
  void Resize(int rows, int columns);

  ParsedRow GetRow(int row_index) const;

 private:
  std::vector<Slot> slots_;
};

class Parser {
 public:
  Parser(int read_fd, epoll::EPoll *epoll);
  ~Parser();

  const ParsedScreen &screen() { return parsed_screen_; }

 private:
  const int read_fd_;
  epoll::EPoll *const epoll_;
  zero_copy::Buffer buffer_;
  ParsedScreen parsed_screen_;
};

}  // namespace ttyrex::parser

#endif  // PARSER_H_
