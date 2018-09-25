// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/Breakpoint.h"

#include <algorithm>
#include <vector>

#include "Core/CPU/CPU.h"

namespace Core::CPU
{
std::vector<Breakpoint> breakpoints;

void AddBreakpoint(Breakpoint b) { breakpoints.push_back(b); }

void RemoveBreakpoint(Breakpoint b)
{
  auto it = std::find_if(
      breakpoints.begin(), breakpoints.end(), [b](const Breakpoint& bp) {
        return bp.offset == b.offset && bp.segment == b.segment;
      });

  if (it == breakpoints.end())
    return;

  breakpoints.erase(it);
}

bool IsBreakpointHit() { return IsBreakpoint(CS, IP); }

bool IsBreakpoint(u16 segment, u16 offset)
{
  for (const auto& bp : breakpoints) {
    if (bp.segment == segment && bp.offset == offset)
      return true;
  }

  return false;
}
} // namespace Core::CPU
