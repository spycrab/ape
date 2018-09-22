// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/Machine.h"

#include "Common/Logger.h"

#include "Core/CPU/CPU.h"
#include "Core/HW/FloppyDrive.h"
#include "Core/HW/VGA.h"
#include "Core/TTY.h"

namespace Core::Machine
{
void Init() { HW::VGA::Init(); }

bool BootFloppy()
{
  if (!HW::FloppyDrive::Read(0, 512, Memory::GetPtr<u8>(0x0000, 0x7C00)))
    return false;

  CPU::CS = 0;
  CPU::IP = 0x7C00;

  CPU::Start();

  return true;
}

void Stop() { CPU::Stop(); }

void Pause() { CPU::SetPaused(!CPU::IsPaused()); }

bool BootCOM(const std::string& file, const std::string&& parameters)
{
  std::ifstream ifs(file, std::ios::binary);

  if (!ifs.good())
    return false;

  CPU::DS = 0;
  CPU::IP = 0x100;
  CPU::simulate_msdos = true;

  size_t index;
  for (index = 0; !ifs.eof(); index++) {
    Memory::Get<u8>(0x0000, 0x0100 + index) = static_cast<u8>(ifs.get());
  }

  LOG("Loaded " + std::to_string(index - 1) + " bytes into memory");

  Memory::Get<u8>(0x0000, 0x0080) = parameters.size();

  u16 offset;

  for (offset = 0x0081; offset < parameters.length(); offset++)
    Memory::Get<char>(0x0000, offset) = parameters[offset - 0x0081];

  Memory::Get<char>(0x0000, offset) = '\0';

  LOG("Command line parameters are \"" + parameters + "\"");

  ifs.close();

  CPU::Start();

  return true;
}
} // namespace Core::Machine
