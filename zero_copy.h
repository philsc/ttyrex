#ifndef ZERO_COPY_H_
#define ZERO_COPY_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <span>

namespace ttyrex {
namespace zero_copy {

class Writer {
 public:
  virtual ~Writer() {}
  virtual std::span<uint8_t> Get(int size) = 0;
  virtual void Rewind(int size) = 0;
};

class Reader {
 public:
  virtual ~Reader() {}
  virtual std::span<const uint8_t> Get(int size) = 0;
};

class Buffer {
 public:
  Buffer(int size);
  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&) = default;

  int bytes_written() const { return bytes_written_; }

  std::unique_ptr<Writer> MakeWriter();
  std::unique_ptr<Reader> MakeReader();

 private:
  class BufferWriter;
  class BufferReader;

  int available() { return storage_.size() - bytes_written_; }

  std::vector<uint8_t> storage_;
  int read_head_ = 0;
  int bytes_written_ = 0;

  friend class BufferWriter;
  friend class BufferReader;
};

}  // namespace zero_copy
}  // namespace ttyrex

#endif  // ZERO_COPY_H_
