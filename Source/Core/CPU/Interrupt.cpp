// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

using namespace Core::CPU;

void CPU::CallInterrupt(u8 vector)
{
  // TODO: Our Handlers / Software Handlers
  // TODO: Actually emulate / use interrupt lookups

  CallBIOSInterrupt(vector);
}
