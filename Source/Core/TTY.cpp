// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/TTY.h"

#include <iostream>

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/TTYBackend.h"

TTYBackend* g_TTYBackend = nullptr;

void TTY::Init() {}

void TTY::Write(const std::string& string)
{
  if (g_TTYBackend)
    g_TTYBackend->Write(string);
  else
    LOG("[TTY STUB] STRING: " + string);
}

void TTY::Write(const char c)
{
  if (g_TTYBackend)
    g_TTYBackend->Write(c);
  else
    LOG("[TTY STUB] CHAR: " + std::string(1, c));
}

void TTY::Scroll(const u8 lines, const u8 color)
{
  if (g_TTYBackend)
    g_TTYBackend->Scroll(lines, color);
  else
    LOG("[TTY STUB] Scroll " + String::ToHex(lines) +
        " C: " + String::ToHex(color));
}
void TTY::MoveCursor(const u32 x, const u32 y)
{
  if (g_TTYBackend)
    g_TTYBackend->MoveCursor(x, y);
  else
    LOG("[TTY STUB] MoveCursor");
}
void TTY::Clear()
{
  if (g_TTYBackend)
    g_TTYBackend->Clear();
  else
    LOG("[TTY STUB] Clear");
}

char TTY::Read()
{
  if (g_TTYBackend) {
    return g_TTYBackend->Read();
  } else {
    LOG("[TTY STUB] Read");
    return 'A';
  }
}
