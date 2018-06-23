// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include <fstream>
#include <iostream>

#include "Common/Logger.h"
#include "Common/String.h"
#include "Common/Types.h"

#include "Core/CPU/Exception.h"
#include "Core/MSDOS/File.h"
#include "Core/Machine.h"

using namespace Core::CPU;
using namespace Core::MSDOS;

bool CPU::CallMSDOSInterrupt(u8 vector)
{
  switch (vector) {
  case 0x21: {
    switch (AH) {
    case 0x19: // Get Default drive
      AL = 0;
      break;
    case 0x30: // Get DOS version
      // Pretend to be MS-DOS 5
      AL = 5;
      AH = 0;
      break;
    case 0x3D: { // Open file
      auto handle = File::Open(m_memory.GetPtr<char>(DS, DX), AL);

      if (handle) {
        AX = handle.value();
      } else {
        AX = 0x01;
      }

      CF = !handle.has_value();
      break;
    }
    default:
      LOG("[INT 0x21] Unhandled parameter AH = " + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    return true;
  }
  default:
    return false;
  }

  return true;
}
