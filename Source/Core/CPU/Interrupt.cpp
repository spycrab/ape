// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include "Common/Logger.h"
#include "Common/String.h"
#include "Core/CPU/Exception.h"
#include "Core/Machine.h"

using namespace Core::CPU;

void CPU::CallInterrupt(u8 vector)
{
  // TODO: Our Handlers / Software Handlers
  // TODO: Actually emulate / use interrupt lookups

  CallBIOSInterrupt(vector);
}

void CPU::CallBIOSInterrupt(u8 vector)
{
  switch (vector) {
  case 0x10: // Video services
    switch (AH) {
    case 0x0E: { // Write character and move cursor
      LOG("OUTPUT: " + std::string(1, AL));
      std::ofstream ofs("output.txt", std::ios::app);
      ofs.put(AL);
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

      u8* dest = &m_memory.Get<u8>(ES, BX);

      LOG("C:H:S = " + String::ToHex<u8>(cylinder) + ":" +
          String::ToHex<u8>(head) + ":" + String::ToHex<u8>(sector));

      LOG("ES:BX = " + String::ToHex<u16>(ES) + ":" + String::ToHex<u16>(BX));

      LOG(String::ToHex<u32>(sector_count) + " to be read.");

      // This is presuming a 360K 5.25" floppy disc
      // TODO: Move more of this logic into FloppyDrive.cpp
      const auto total_sector = (cylinder * 2 + head) * 9 + (sector - 1);

      LOG("Sector = " + String::ToHex<u32>(total_sector));
      LOG("Address = " + String::ToHex<u32>(total_sector * 512));

      if (!m_machine->GetFloppyDrive().Read(total_sector * 512,
                                            sector_count * 512, dest)) {
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
      LOG("Key requested.");
      AL = static_cast<u8>(std::cin.get());
      AH = 0; // TODO: This should be the scancode...
      CF = false;
      break;
    default:
      LOG("[INT 16h] Unknown parameter AH=" + String::ToHex(AH));
      throw UnhandledInterruptException();
    }
    break;
  }
  default:
    LOG("[CBI] Unknown vector " + String::ToHex(vector));
    throw UnhandledInterruptException();
  }
}
