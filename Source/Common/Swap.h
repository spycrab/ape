// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

template <typename T> T Swap(T data)
{
  T x = 0;

  for (size_t i = 0; i < sizeof(T); i++) {
    int d = (data >> (i * 8)) & 0xff;
    x |= (d << (sizeof(T) * 8 - 8 - i * 8));
  }

  return x;
}
