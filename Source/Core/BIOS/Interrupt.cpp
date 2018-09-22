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
#include "Core/HW/FloppyDrive.h"
#include "Core/TTY.h"

using namespace Core;

bool CPU::CallBIOSInterrupt(u8 vector)
{
  switch (vector) {
  case 0x10: // Video services
    switch (AH) {
    case 0x06: { // Scroll screen
      TTY::Scroll(BL, BH);
      break;
    }
    case 0x02: { // Set cursor
      TTY::SetCursorRow(DH);
      TTY::SetCursorColumn(DL);
      break;
    }
    case 0x03: { // Get cursor info
      AX = 0;

      // TODO: Start and end scan line?
      CH = CL = 0;

      DH = TTY::GetCursorRow();
      DL = TTY::GetCursorColumn();
      break;
    }
    case 0x0E: { // Write character and move cursor
      TTY::Write(AL);
    } break;
    default:
      LOG("[INT 10h] Unknown parameter AH=" + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    break;
  case 0x13: // Disc services
    switch (AH) {
    case 0x00: // Reset disc drives
      // TODO: Do something here?
      AH = 0;
      CF = false;
      break;
    case 0x02: { // Read disc sector

      const u8 sector_count = AL;
      const u8 cylinder = CH;
      const u8 sector = CL;
      const u8 head = DH;
      const u8 drive = DL;

      // We currently only support one floppy drive.
      if (drive != 0) {
        AH = 0xAA; // Drive not ready
        CF = true;
        break;
      }

      u8* dest = Memory::GetPtr<u8>(ES, BX);

      LOG("C:H:S = " + String::ToHex<u8>(cylinder) + ":" +
          String::ToHex<u8>(head) + ":" + String::ToHex<u8>(sector));

      LOG("ES:BX = " + String::ToHex<u16>(ES) + ":" + String::ToHex<u16>(BX));

      LOG(String::ToHex<u32>(sector_count) + " to be read.");

      if (!Core::HW::FloppyDrive::Read(cylinder, head, sector, sector_count,
                                       dest)) {
        AH = 0x40; // Bad seek (Is there a more fitting one?)
        CF = true;
        LOG("Read error!");
        break;
      }

      AH = 0;
      CF = false;
      break;
    }
    default:
      LOG("[INT 13h] Unknown parameter AH=" + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    break;
  case 0x16: { // Keyboard services
    switch (AH) {
    case 0x00: // Wait for key press
      AL = TTY::Read();
      AH = 0; // TODO: This should be the scancode...
      CF = false;
      break;
    default:
      LOG("[INT 16h] Unknown parameter AH=" + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    break;
  case 0x17: // Printer services
    switch (AH) {
    case 0x00: // Write character
      LOG("[PRINTER STUB] Writing " + std::string(AL, 1));

      // Set both the "Out of paper" and "Selected" flag to indicate no printer
      // is attached
      AH = 0b0011'0000;
      break;
    default:
      LOG("[INT 17h] Unknown parameter AH=" + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    break;
  }
  case 0x19: // Reset
  {
    LOG("[STUB] Reset requested; Stopping...");
    Stop();
    break;
  }
  default:
    return false;
  }

  return true;
}
