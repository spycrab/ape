// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/TTY.h"

#include <iostream>

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/HW/VGA.h"

static u8 s_column = 0;
static u8 s_row = 1;

void TTY::Write(const std::string& string)
{
  for (char c : string)
    Write(c);
}

void TTY::Write(const char c)
{
  if (c == '\n') {
    s_row++;
    return;
  }

  if (c == '\r') {
    s_column = 0;
    return;
  }

  if (c == '\b') {
    s_column--;
    return;
  }

  // TODO: Implement bell
  if (c == '\a')
    return;

  if (!Core::HW::VGA::IsPresent()) {
    LOG("[TTY STUB] CHAR: " + std::string(1, c));
    return;
  }

  Core::HW::VGA::GetBuffer()[((s_row - 1) * 80 + s_column) * sizeof(u16)] = c;
  s_column++;

  s_column %= 80;
}

void TTY::Scroll(const u8 lines, const u8 color)
{
  LOG("[TTY STUB] Scroll " + String::ToHex(lines) +
      " C: " + String::ToHex(color));
}
void TTY::MoveCursor(const u32 x, const u32 y)
{
  if (!Core::HW::VGA::IsPresent()) {
    LOG("[TTY STUB] MoveCursor to " + std::to_string(x) + "," +
        std::to_string(y));
    return;
  }

  s_column = x;
  s_row = y;
}
void TTY::Clear()
{
  if (!Core::HW::VGA::IsPresent()) {
    LOG("[TTY STUB] Clear");
    return;
  }

  for (size_t y = 0; y < 80; y++) {
    for (size_t x = 0; x < 80; x++) {
      Core::HW::VGA::GetBuffer()[(y * 80 + x) * sizeof(u16)] = 0;
    }
  }
}

char TTY::Read()
{
  LOG("[TTY STUB] Read");
  return 'A';
}

u8 TTY::GetCursorRow() { return s_row; }

void TTY::SetCursorRow(u8 row) { s_row = row; }

u8 TTY::GetCursorColumn() { return s_column; }

void TTY::SetCursorColumn(u8 column) { s_column = column; }

bool TTY::IsCharAvailable()
{
  LOG("[TTY STUB] Char available");
  return false;
}
