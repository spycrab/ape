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
#include "Core/TTY.h"

using namespace Core;
using namespace Core::MSDOS;

bool CPU::CallMSDOSInterrupt(u8 vector)
{
  switch (vector) {
  case 0x20: // Exit program
    LOG("Exit requested, shutting down...");
    running = false;
    break;
  case 0x21: {
    switch (AH) {
    case 0x02: { // Print char
      TTY::Write(DL);
      break;
    }
    case 0x06: { // Read char
      AL = TTY::Read();
      ZF = false;
      break;
    }
    case 0x07: // Read char (no echo)
      // TODO: Actually does echo
      AL = TTY::Read();
      ZF = false;
      break;
    case 0x09: // Print string
    {
      std::string s = "";

      char* c = Memory::GetPtr<char>(DS, DX);

      while (*c != '$')
        s += *(c++);

      TTY::Write(s);
      break;
    }
    case 0x0b: // See if chars are available in stdin
      AL = TTY::IsCharAvailable();
      break;
    case 0x19: // Get Default drive
      AL = 0;
      break;
    case 0x30: // Get DOS version
      // Pretend to be MS-DOS 5
      LOG("DOS version requested; faking 5.0");
      AL = 5;
      AH = 0;
      break;
    case 0x3D: { // Open file
      auto handle = File::Open(Memory::GetPtr<char>(DS, DX), AL);

      if (handle) {
        AX = handle.value();
      } else {
        AX = 0x01;
      }

      CF = !handle.has_value();
      break;
    }
    case 0x3F: { // Read file
      auto read = File::Read(BX, CX, Memory::GetPtr<u8>(DS, DX));

      if (read) {
        AX = read.value();
      } else {
        AX = 0x05;
      }

      CF = !read.has_value();
      break;
    }
    case 0x42: { // Seek file
      auto offset =
          File::Seek(BX, static_cast<File::SeekOrigin>(AL), CX << 16 | DX);

      if (offset) {
        CX = (offset.value() & 0xFFFF0000) >> 16;
        DX = offset.value() & 0xFFFF;
      } else {
        AX = 0x01;
      }

      CF = !offset.has_value();
      break;
    }
    case 0x4C: // Exit program with return code
      running = false;
      LOG("Program exited with return code " + String::ToHex(AL));
      break;
    case 0x50: // Set PSP
      LOG("[STUB] Set PSP = " + String::ToHex(BX));
      CF = false;
      break;

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
