#include "zero_copy.h"

#include <algorithm>

#include <glog/logging.h>

namespace ttyrex {
namespace zero_copy {

class Buffer::BufferWriter : public Writer {
 public:
  BufferWriter(Buffer *buffer) : buffer_(buffer) {}

  std::span<uint8_t> Get(int size) override {
    const int write_head = (buffer_->read_head_ + buffer_->bytes_written_) %
                           buffer_->storage_.size();

    const int max_continguous_size = [this, write_head]() -> int {
      if (buffer_->available() == 0) {
        return 0;
      } else if (write_head >= buffer_->read_head_) {
        return buffer_->storage_.size() - write_head;
      } else {
        return buffer_->read_head_ - write_head;
      }
    }();

    const int final_size = std::min(size, max_continguous_size);
    buffer_->bytes_written_ += final_size;

    return std::span<uint8_t>(&buffer_->storage_[write_head], final_size);
  }

  void Rewind(int size) override {
    CHECK_LE(size, buffer_->bytes_written_);
    buffer_->bytes_written_ -= size;
  }

 private:
  Buffer *buffer_;
};

class Buffer::BufferReader : public Reader {
 public:
  BufferReader(Buffer *buffer) : buffer_(buffer) {}

  std::span<const uint8_t> Get(int size) override {
    const int max_continguous_size = [this]() -> int {
      if (buffer_->read_head_ + buffer_->bytes_written_ >=
          static_cast<int>(buffer_->storage_.size())) {
        return buffer_->storage_.size() - buffer_->read_head_;
      } else {
        return buffer_->bytes_written_;
      }
    }();

    const int final_size = std::min(size, max_continguous_size);
    const uint8_t *data = &buffer_->storage_[buffer_->read_head_];

    buffer_->read_head_ =
        (buffer_->read_head_ + final_size) % buffer_->storage_.size();
    buffer_->bytes_written_ -= final_size;

    return std::span<const uint8_t>(data, final_size);
  }

 private:
  Buffer *buffer_;
};

Buffer::Buffer(int size) : storage_(size) { CHECK_GE(size, 1); }

std::unique_ptr<Writer> Buffer::MakeWriter() {
  return std::make_unique<BufferWriter>(this);
}
std::unique_ptr<Reader> Buffer::MakeReader() {
  return std::make_unique<BufferReader>(this);
}

}  // namespace zero_copy
}  // namespace ttyrex
