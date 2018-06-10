#include <gtest/gtest.h>

#include "Common/String.h"

TEST(String, ToHex)
{
  ASSERT_STREQ(String::ToHex<u8>(0xFF).c_str(), "0xff");
  ASSERT_STREQ(String::ToHex<u16>(256).c_str(), "0x0100");
  ASSERT_STREQ(String::ToHex<u32>(0xFFFF0000).c_str(), "0xffff0000");
  ASSERT_STREQ(String::ToHex<u64>(0x1234567890ABCDEF).c_str(),
               "0x1234567890abcdef");
}

TEST(String, ToBin)
{
  ASSERT_STREQ(String::ToBin<u8>(0b11111111).c_str(), "11111111");
  ASSERT_STREQ(String::ToBin<u8>(0b00001111).c_str(), "00001111");
  ASSERT_STREQ(String::ToBin<u16>(0xFF00).c_str(), "1111111100000000");
}
