// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <string>

#include "Common/Types.h"

#pragma once

class TTYBackend;

extern TTYBackend* g_TTYBackend;

namespace TTY
{
void Init();
void Write(const std::string& string);
void Write(const char c);
void Scroll(const u8 lines, const u8 color);
void MoveCursor(const u32 x, const u32 y);
u8 GetCursorRow();
void SetCursorRow(u8 row);
u8 GetCursorColumn();
void SetCursorColumn(u8 column);
void Clear();
char Read();
}
