#include <gtest/gtest.h>
#include <utf8.h>

TEST(Utf8, Next) {
  char8_t twochars[] = u8"\xe6\x97\xa5\xd1\x88";
  char8_t* w = twochars;
  int cp = utf8::next(w, twochars + 6);
  ASSERT_EQ(cp, 0x65e5);
  ASSERT_EQ(w, twochars + 3);
}
