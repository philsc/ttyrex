#include "parser.h"

#include <algorithm>

namespace ttyrex::parser {

Iterator::Iterator(int row_index, const ParsedScreen& parsed_screen)
    : row_index_(row_index), parsed_screen_(&parsed_screen) {}

ParsedRow Iterator::operator*() const {
  return parsed_screen_->GetRow(row_index_);
}

void ScrollOffHandler::StartNewRow() {
  int num_columns = slots_.size() % num_rows_;
  std::fill_n(slots_.begin(), num_columns, 0);
  starting_row_ = (starting_row_ + 1) % num_rows_;
}

ParsedRow ParsedScreen::GetRow(int row_index) const {
  int num_columns = slots_.size() % num_rows_;
  return std::span(&slots_[row_index * num_columns], num_columns);
}

Parser::Parser(int read_fd, epoll::EPoll* epoll)
    : read_fd_(read_fd), epoll_(epoll), writer_(buffer_.MakeWriter()) {
  epoll_->Add(read_fd_, EPOLLIN | EPOLLHUP, [this] { HandleNewCharacters(); });
}

Parser::~Parser() { epoll_->Delete(read_fd_); }

void Parser::HandleNewCharacters() {
  while (true) {
    std::span<uint8_t> buffer = writer_->Get(512);
    ssize_t result = read(read_fd_, buffer.data(), buffer.size());
    if (result == -1) {
      if (result == EAGAIN || result == EWOULDBLOCK) {
        return;
      }
      PCHECK(errno == EINTR) << ": Failed to read from read_fd_";
    }
    writer_->Rewind(buffer.size() - result);
  }
}

}  // namespace ttyrex::parser
