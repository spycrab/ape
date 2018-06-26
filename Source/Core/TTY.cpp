// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/TTY.h"

#include <iostream>

#include "Common/Logger.h"
#include "Common/String.h"

void TTY::Init() {}

void TTY::Write(const std::string& string)
{
  LOG("[TTY STUB] STRING: " + string);
}

void TTY::Write(const char c) { LOG("[TTY STUB] CHAR: " + std::string(c, 1)); }

void TTY::Scroll(const u8 lines, const u8 color)
{
  LOG("[TTY STUB] Scroll " + String::ToHex(lines) +
      " C: " + String::ToHex(color));
}
void TTY::MoveCursor(const u32, const u32) { LOG("[TTY STUB] MoveCursor"); }
void TTY::Clear() { LOG("[TTY STUB] Clear"); }

char TTY::Read()
{
  LOG("[TTY STUB] Read");
  return 'A';
}
