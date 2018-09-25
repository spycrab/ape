// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "Common/Types.h"

namespace String
{
template <typename T> std::string ToHex(const T& value)
{
  std::stringstream ss;
  ss << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex
     << static_cast<i64>(value);
  return ss.str();
}

template <typename T> std::string ToBin(T value)
{
  std::string bin;

  for (int i = (sizeof(T) * 8) - 1; i > -1; i--) {
    bin += (value & (1 << i)) ? "1" : "0";
  }

  return bin;
}
} // namespace String
