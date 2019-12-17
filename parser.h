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

class RowIterator {
 public:
  using difference_type = int;
  using value_type = ParsedRow;
  using pointer = ParsedRow *;
  using reference = ParsedRow;
  using iterator_category = std::forward_iterator_tag;

  RowIterator(int row_index, const ParsedScreen &parsed_screen);
  RowIterator(const RowIterator &) = default;

  RowIterator &operator=(const RowIterator &) = default;
  auto operator<=>(const RowIterator &other) {
    return row_index_ - other.row_index_;
  }
  RowIterator &operator++() {
    ++row_index_;
    return *this;
  }
  ParsedRow operator*() const;

 private:
  int row_index_;
  const ParsedScreen *parsed_screen_;
};

class ScrollOffHandler {
 public:
   void StartNewRow();

 private:
  int num_rows_;
  int starting_row_;
  std::vector<Slot> slots_;
};

class ParsedScreen {
 public:
  ParsedRow GetRow(int row_index) const;

  RowIterator begin() { return RowIterator(0, *this); }
  RowIterator end() { return RowIterator(num_rows_, *this); }
  RowIterator cbegin() const { return RowIterator(0, *this); }
  RowIterator cend() const { return RowIterator(num_rows_, *this); }

 private:
  int num_rows_;
  std::vector<Slot> slots_;
};

class Parser {
 public:
  Parser(int read_fd, epoll::EPoll *epoll);
  ~Parser();

  void Resize(int rows, int columns);

  const ParsedScreen &screen() { return parsed_screen_; }

 private:
  void HandleNewCharacters();

  const int read_fd_;
  epoll::EPoll *const epoll_;
  zero_copy::Buffer buffer_;
  std::unique_ptr<zero_copy::Writer> writer_;
  ParsedScreen parsed_screen_;
};

}  // namespace ttyrex::parser

#endif  // PARSER_H_
