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
  // TODO(phil): Change to a Consume API? It's kind of annoying to use this
  // way.
  virtual void Rewind(int size) = 0;
};

class Reader {
 public:
  virtual ~Reader() {}
  virtual std::span<const uint8_t> Get(int size) = 0;
};

template <typename T>
class CircularBuffer {
 public:
  CircularBuffer(int capacity) : storage_(capacity) {}

  void Add(std::span<const T> data) {
    while (data.size() > 0) {
      int write_head = (read_head_ + bytes_written_) % storage_.size();
      int contiguous_write_size = storage_.size() - write_head;
      const int num_bytes_to_copy = std::min<int>(contiguous_write_size, data.size());
      memcpy(&storage_[write_head], data.data(), num_bytes_to_copy);
      data = data.subspan(num_bytes_to_copy);
      bytes_written_ = std::min<int>(bytes_written_ + num_bytes_to_copy, storage_.size());
      if (bytes_written_ == static_cast<int>(storage_.size())) {
        read_head_ = (write_head + 1) % storage_.size();
      }
    }
    new_data_available_ = true;
  }

  bool NewDataIsAvailable() { return std::exchange(new_data_available_, false); }

  int size() const { return bytes_written_; }

  T &operator[](int position) {
    int index = (read_head_ + position) % storage_.size();
    return storage_[index];
  }

  const T &operator[](int position) const {
    int index = (read_head_ + position) % storage_.size();
    return storage_[index];
  }

 private:
  std::vector<T> storage_;
  int read_head_ = 0;
  int bytes_written_ = 0;
  bool new_data_available_ = false;
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
