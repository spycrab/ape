// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include "Common/Types.h"

namespace Core::CPU
{
struct Breakpoint {
  u16 segment, offset;
};

void AddBreakpoint(Breakpoint b);
void RemoveBreakpoint(Breakpoint b);

bool IsBreakpointHit();
bool IsBreakpoint(u16 sgement, u16 offset);
} // namespace Core::CPU
