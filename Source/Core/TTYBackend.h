// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <string>

#include "Common/Types.h"

class TTYBackend
{
public:
  virtual void Write(const std::string& string) = 0;
  virtual void Write(const char c) = 0;
  virtual void Scroll(const u8 lines, const u8 colors) = 0;
  virtual void MoveCursor(const u32, const u32) = 0;
  virtual u8 GetCursorRow() const = 0;
  virtual void SetCursorRow(u8 row) = 0;
  virtual u8 GetCursorColumn() const = 0;
  virtual void SetCursorColumn(u8 column) = 0;
  virtual void Clear() = 0;
  virtual char Read() = 0;
  virtual bool IsCharAvailable() const = 0;
};
