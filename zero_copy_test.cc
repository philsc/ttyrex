#include <span>
#include <string_view>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <range/v3/view/iota.hpp>

#include "zero_copy.h"

namespace ttyrex {
namespace zero_copy {
namespace testing {

TEST(ZeroCopyTest, Writer) {
  Buffer buffer(10);
  auto writer = buffer.MakeWriter();
  auto write_buffer = writer->Get(20);
  ASSERT_EQ(write_buffer.size(), 10);

  writer->Rewind(5);
  write_buffer = writer->Get(20);
  ASSERT_EQ(write_buffer.size(), 5);

  write_buffer = writer->Get(20);
  ASSERT_EQ(write_buffer.size(), 0);
}

TEST(ZeroCopyTest, Reader) {
  Buffer buffer(10);
  auto reader = buffer.MakeReader();
  auto read_buffer = reader->Get(20);
  ASSERT_EQ(read_buffer.size(), 0);
}

namespace {

void AddDataToBuffer(std::u8string_view data, Writer *writer) {
  auto write_buffer = writer->Get(data.size());
  ASSERT_EQ(write_buffer.size(), data.size());
  memcpy(write_buffer.data(), data.data(), data.size());
}

}  // namespace

TEST(ZeroCopyTest, WriterAndReader) {
  Buffer buffer(97);
  auto writer = buffer.MakeWriter();
  auto reader = buffer.MakeReader();

  AddDataToBuffer(u8"Hello", writer.get());
  AddDataToBuffer(u8"World", writer.get());

  auto read_buffer = reader->Get(20);
  ASSERT_EQ(read_buffer.size(), 10);
  ASSERT_EQ(memcmp(read_buffer.data(), u8"HelloWorld", read_buffer.size()), 0);
}

TEST(ZeroCopyTest, LotsOfWritingAndLittleReading) {
  // TODO(phil): Implement this.
  static constexpr uint8_t kWriteSequence[] = {1, 2, 3, 4, 5, 6, 7};
  int write_index = 0;
  uint8_t expected_read = 1;

  Buffer buffer(100);
  auto writer = buffer.MakeWriter();
  auto reader = buffer.MakeReader();

  for (int i : ranges::views::iota(0, 1000)) {
    SCOPED_TRACE(i);
    auto write_buffer = writer->Get(std::size(kWriteSequence) - write_index);
    ASSERT_GE(write_buffer.size(), 1);
    memcpy(write_buffer.data(), kWriteSequence + write_index,
           write_buffer.size());
    write_index =
        (write_index + write_buffer.size()) % std::size(kWriteSequence);

    auto read_buffer = reader->Get(3);
    ASSERT_GE(read_buffer.size(), 1);
    for (auto data : read_buffer) {
      EXPECT_EQ(data, expected_read);
      expected_read = (expected_read % std::size(kWriteSequence)) + 1;
    }
  }
}

}  // namespace testing
}  // namespace zero_copy
}  // namespace ttyrex
